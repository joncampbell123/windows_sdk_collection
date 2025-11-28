//-----------------------------------------------------------------------------
// File: Fractool.cs
//
// Desc: Create fractals.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;




namespace FractalTool
{
	/// <summary>
	/// This class generates a 2D array of elevation points using
	/// midpoint displacement and random additions in two dimensions.
	/// </summary>
	public class ElevationPoints
	{
		//fractal terrain goes here. It is (2^maxlevel+1)^2 in bufferSize.
		public double[,] X;
		//These values govern the topology of the fractal mesh
		private int maxlevel;
		private bool addition;
		private double sigma;
		private double shape;

		//Gausian number generator.
		private FractalTool.GaussGen Gauss;
		private double f3(double delta, double x0, double x1,double x2){return ((x0+x1+x2)/3+delta *Gauss.GaussianNumber);}
		private double f4(double delta, double x0, double x1,double x2,double x3){return ((x0+x1+x2+x3)/4+delta *Gauss.GaussianNumber);}

		/// <summary>
		/// Constrcutor. Pass in parameters
		/// </summary>
		/// <param name="maxLevel"> Maxlevel : determines the bufferSize of the fractal mesh</param>
		/// <param name="add"> Use random additions?</param>
		/// <param name="sd"> sigma : initial standard deviation</param>
		/// <param name="fdim"> fractal dimenion. Determines general shape of mesh</param>
		public ElevationPoints(int maxLevel, bool add, double sd, double fdim)
		{
			maxlevel = maxLevel;
			addition = add;
			sigma = sd;
			shape = fdim;
		}

		/// <summary>
		/// Constructor : uses arbitrary defualts
		/// </summary>
		public ElevationPoints()
		{
			maxlevel = 5;
			addition = true;
			sigma = .5;
			shape = .5;
		}

		/// <summary>
		/// Generates a fractal mesh 2^maxelvel+1 in bufferSize
		/// cribbed from "The Science of Fractal Images"
		/// </summary>
		public void CalcMidpointFM2D()
		{
			double delta;     //tracks standard deviation
			int N, stage;     //Integers
			int x,y,D,d;      //Array indices

			//Initialize gaussian number widget
			Gauss = new FractalTool.GaussGen();
			N = (int) Math.Pow(2,maxlevel);
			delta = sigma;

			//Allocate dump for data
			X = new double[N+1,N+1];
			//Init starting corner points in grid
			X[0,0] = delta*Gauss.GaussianNumber;
			X[0,N] = delta*Gauss.GaussianNumber;
			X[N,0] = delta*Gauss.GaussianNumber;
			X[N,N] = delta*Gauss.GaussianNumber;
			D = N;
			d = N/2;
			stage = 1;

			while(stage<=maxlevel)
			{
				delta = delta*Math.Pow(0.5,0.5*shape);
				for (x=d;x<=N-d;x+=D)
					for (y=d;y<=N-d;y+=D)
						X[x,y]=f4(delta,X[x+d,y+d],X[x+d,y-d],X[x-d,y+d],X[x-d,y-d]);

				if (addition)
					for (x=0;x<=N;x+=D)
						for (y=0;y<=N;y+=D)
							X[x,y]=X[x,y]+delta*Gauss.GaussianNumber;


				delta = delta*Math.Pow(0.5,0.5*shape);

				for (x=d;x<=N-d;x+=D)
				{
					X[x,0] = f3(delta,X[x+d,0],X[x-d,0],X[x,d]);
					X[x,N] = f3(delta,X[x+d,N],X[x-d,N],X[x,N-d]);
					X[0,x] = f3(delta,X[0,x+d],X[0,x-d],X[d,x]);
					X[N,x] = f3(delta,X[N,x+d],X[N,x-d],X[N-d,x]);
				}

				for (x=d;x<=N-d;x+=D)
					for (y=D;y<=N-d;y+=D)
						X[x,y] = f4(delta,X[x,y+d],X[x,y-d],X[x+d,y],X[x-d,y]);

				for (x=D;x<=N-d;x+=D)
					for (y=d;y<=N-d;y+=D)
						X[x,y] = f4(delta,X[x,y+d],X[x,y-d],X[x+d,y],X[x-d,y]);


				if (addition)
				{
					for (x=0;x<=N;x+=D)
						for (y=0;y<=N;y+=D)
							X[x,y]=X[x,y]+delta*Gauss.GaussianNumber;


					for (x=d;x<=N-d;x+=D)
						for (y=d;y<=N-d;y+=D)
							X[x,y]=X[x,y]+delta*Gauss.GaussianNumber;
				}

				D=D/2;
				d=d/2;
				stage++;
			}
		}
	}

	class GaussGen
	{
		private int Arand;
		private double GaussAdd, numer, denom;
		private Random rand;

		/// <summary>
		/// Constructor; Initialize the Gausian number system
		/// </summary>
		/// <param name="seed"></param>
		public GaussGen() 
		{
			rand = new Random(unchecked((int)DateTime.Now.Ticks));
			Arand=(int)Math.Pow(2,31)-1;
			GaussAdd = Math.Sqrt(12);
			numer    = GaussAdd + GaussAdd;
			denom    = (double)4*Arand;
		}

		/// <summary>
		/// Return a Gaussian number
		/// </summary>
		/// <returns></returns>
		public double GaussianNumber 
		{
			get
			{
				int i;
				double sum = 0;
				for (i=1;i<=4;i++)
					sum+=rand.Next(Arand);
				return((sum*numer/denom)-GaussAdd);
			}
		}
	}
}


