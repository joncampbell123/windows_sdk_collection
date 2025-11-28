//-----------------------------------------------------------------------------
// File: Chart.cs
//
// Desc: Chart control
//
// Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;


namespace ActionBasic 
{
    /// <summary>
	/// Summary description for Chart.
	/// </summary>
    public class Chart : System.Windows.Forms.Panel
    {
        private const int GutterSize     = 10;   // Chart display constants
        private const int CellSize       = 15;
        
        // Stored data to be drawn
        private String[] ActionNames = null;
        private ArrayList DeviceStates = null;

        private bool RefreshNeeded = true;
        
        private Rectangle ChartRect = new Rectangle(0, 0, 0, 0);  // Bounding RECT for the chart (grid and titles)
        private Rectangle GridRect  = new Rectangle(0, 0, 0, 0);  // Bounding RECT for the grid
       
        Brush   ActiveBrush   = new SolidBrush( Color.FromKnownColor( KnownColor.Highlight ) );
        Brush   UnmappedBrush = new HatchBrush( HatchStyle.BackwardDiagonal, Color.FromKnownColor( KnownColor.Highlight ), Color.FromKnownColor( KnownColor.Control ) );
        Brush   InactiveBrush = new SolidBrush( Color.FromKnownColor( KnownColor.Control ) );
        Pen     GridPen       = new Pen( Color.FromKnownColor( KnownColor.ControlDark ) );
        Pen     AxisPen       = new Pen( Color.FromKnownColor( KnownColor.Control ) );
        
        private Font ActionFont = new Font("arial", 9, FontStyle.Regular, GraphicsUnit.Pixel, 0, true );
        private Font DeviceFont = new Font("arial", 9, FontStyle.Regular, GraphicsUnit.Pixel, 0, false );

        public Chart()
        {
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
        }

        public String[] ColumnTitles
        {
            get
            {
                return ActionNames;
            }
            set
            {
                ActionNames = value;
                RefreshNeeded = true;
                Invalidate();
            }
        }

        public ArrayList RowData
        {
            get
            {
                return DeviceStates;
            }
            set
            {
                DeviceStates = value;
                RefreshNeeded = true;
                Invalidate();
            }
        }

        

        public void UpdateData()
        {
            int iY = GridRect.Top;
    
            foreach( DeviceState state in DeviceStates )
            { 
                Rectangle rc = new Rectangle( GridRect.Left + 3, iY + 3, 10, 10 ); 

                for( int i=0; i < state.InputState.Length; i++ )
                {
                    if( state.InputState[i] != state.PaintState[i] )
                        Invalidate( rc );

                    rc.Offset( CellSize, 0 );
                }
                iY += CellSize; 
            }
            Invalidate(false);            
        }

        protected void PaintChart( Graphics g )
        {
            int iY = GridRect.Top;
            
            foreach( DeviceState state in DeviceStates )
            { 
                Rectangle rc = new Rectangle( GridRect.Left + 3, iY + 3, 10, 10 ); 

                for( int i=0; i < state.InputState.Length; i++ )
                {
                    if( state.IsMapped[i] == false )
                    {
                        g.FillRectangle( UnmappedBrush, rc );
                    }
                    else if( state.InputState[i] != 0 )
                    {
                        g.FillRectangle( ActiveBrush, rc );

                        if( (GameActions)i == GameActions.Walk )
                        {
                            // Scale the axis data to the size of the cell 
                            int tempX = rc.Left+1 + (state.InputState[i] + 100)/25; 

                            g.DrawLine( AxisPen, new Point( tempX,   rc.Bottom-2 ), new Point( tempX,   rc.Top ) );
                            g.DrawLine( AxisPen, new Point( tempX-1, rc.Top+2 ),    new Point( tempX+2, rc.Top+2 ) ); 
                            g.DrawLine( AxisPen, new Point( tempX-2, rc.Top+3 ),    new Point( tempX+3, rc.Top+3 ) );
                        }
                    }
                    else
                    {
                        g.FillRectangle( InactiveBrush, rc );
                    }

                    state.PaintState[i] = state.InputState[i];       
                    rc.Offset( CellSize, 0 );
                }

                iY += CellSize; 
            }
        }

        protected void PaintLegend( Graphics g )
        {
            // Paint the legend
            g.DrawString( "Inactive", DeviceFont, Brushes.Black, 25, 8 );
            g.DrawRectangle( GridPen, 8, 8, 12, 12 );

            g.DrawString( "Active", DeviceFont, Brushes.Black, 25, 24 );
            g.DrawRectangle( GridPen, 8, 24, 12, 12 );
            g.FillRectangle( ActiveBrush, 10, 26, 9, 9 );

            g.DrawString( "Unmapped", DeviceFont, Brushes.Black, 25, 40 );
            g.DrawRectangle( GridPen, 8, 40, 12, 12 );
            g.FillRectangle( UnmappedBrush, 10, 42, 9, 9 );
        }

        protected void PaintGrid( Graphics g )
        {
            int iX = GridRect.Left;
            int iY = GridRect.Top - GutterSize;

            StringFormat formatAction = new StringFormat( StringFormatFlags.DirectionVertical );
            formatAction.Alignment = StringAlignment.Far;
            foreach( String action in ActionNames )
            {
                g.DrawString( action, ActionFont, Brushes.Black, iX+2, iY, formatAction );
                g.DrawRectangle( Pens.Gray, iX, GridRect.Top, CellSize, GridRect.Bottom - GridRect.Top );

                iX += CellSize;
            }
    
            iY = GridRect.Top;

            foreach( DeviceState state in DeviceStates )
            {
                g.DrawString( state.Name, DeviceFont, Brushes.Black, GridRect.Left - GutterSize, iY+1, new StringFormat( StringFormatFlags.DirectionRightToLeft ) );
                g.DrawRectangle( Pens.Gray, GridRect.Left, iY, GridRect.Right - GridRect.Left, CellSize );
            
                iY += CellSize; 
            }
        }

        /// <summary>
        /// Override of the paint method. All of the code in this method is
        /// devoted to correctly positioning the chart and drawing labels.
        /// </summary>
        /// <seealso>PaintChart</seealso>
        /// <param name="e">Paint Argumenets</param>
        protected override void OnPaint( PaintEventArgs e )
        {
            base.OnPaint( e );

            Graphics g = e.Graphics;

            if( null == ActionNames || 
                null == DeviceStates )
            {
                return;
            }
            
            try
            {
                if( RefreshNeeded )
                    RefreshChart( g );

                PaintLegend( g );   
                PaintGrid( g );
                PaintChart( g );
            }
            catch(System.InvalidOperationException)
            {
                OnPaint(e);
                Application.DoEvents();
            }
        }

        private void RefreshChart( Graphics g )
        {
            RefreshNeeded = false;

            int iMaxActionSize=0, iMaxDeviceSize=0;   

            // Determine the largest action name size
            foreach( string name in ActionNames )
            {
                Size size = ( g.MeasureString( name, ActionFont ) ).ToSize();
                iMaxActionSize = Math.Max( iMaxActionSize, size.Width );
            }

            // Determine the largest device name size
            foreach( DeviceState state in DeviceStates )
            {
                Size size = ( g.MeasureString( state.Name, DeviceFont ) ).ToSize();
                iMaxDeviceSize = Math.Max( iMaxDeviceSize, size.Width );
            }

            // Determine the bounding rectangle for the chart and grid
            GridRect.Width  = CellSize * ActionNames.Length;
            GridRect.Height = CellSize * DeviceStates.Count;

            ChartRect.Width = GridRect.Width + iMaxDeviceSize + GutterSize;
            ChartRect.Height = GridRect.Height + iMaxActionSize + GutterSize;

            ChartRect.X      = ( this.Width  - ChartRect.Width  ) / 2;
            ChartRect.Y      = ( this.Height - ChartRect.Height ) / 2;

            GridRect.X = ChartRect.X + iMaxDeviceSize + GutterSize;
            GridRect.Y = ChartRect.Y + iMaxActionSize + GutterSize;
        }
	}
}
