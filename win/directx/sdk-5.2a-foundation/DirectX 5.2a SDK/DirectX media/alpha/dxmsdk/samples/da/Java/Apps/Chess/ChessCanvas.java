import com.ms.dxmedia.*;
import ChessModel;


// AWT component that displays the animation
public class ChessCanvas extends DXMCanvas
{
  
  protected ChessModel m_model = null;

  ChessCanvas(ChessModel model) {
    m_model = model;
    setModel(m_model);
  }
  
  // This method allows the model to know about the size of the canvas. 
  public void reshape(int x, int y, int width, int height) {
    super.reshape(x,y,width,height);
    m_model.SetSize(width,height);
  }


}

