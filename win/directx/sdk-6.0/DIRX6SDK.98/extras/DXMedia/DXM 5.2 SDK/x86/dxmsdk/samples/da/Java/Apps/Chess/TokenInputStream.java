import java.io.*;

/*
 *
 * TokenInputStream
   This class will read until a token is met, and will 
   eat all whitespace.
 *
 */
public class TokenInputStream extends FilterInputStream
{
	protected InputStream in;
	 protected byte buf[] = new byte[256];
	 protected int buf_offset = 0;
	 protected int buf_left = 0;
	 protected char white[] = null;

	 public TokenInputStream(InputStream input) {
		 super(input);
		 in = input;
		setWhite("\r\n\t ");

	 }
	 
	 public void setWhite(String s) {
		 white = new char[s.length()];
		 s.getChars(0,s.length(),white,0);		 
	 }

		
	 public int read(char b[], int  off, int  len) throws IOException{
			
		 int copied = 0;
		 if (buf_left == -1)
			 return -1;
		 
				
		 while (len > 0) {	
			if (buf_left == 0) {
			// copy more into buffer	
				buf_offset = 0;
				buf_left = in.read(buf, 0, buf.length);
				if (buf_left < 0)
					return copied;
			}
			
			// for each char in the buffer
			for (;buf_left > 0; buf_offset++, buf_left--){
				boolean isToken = false;
				// compare with white
				for (int j = 0; j < white.length; j++) {
					if (buf[buf_offset] == white[j]){
						isToken = true;
						break;
					}
				}
				if (isToken && (copied != 0))
					return copied;
				b[off] =(char) buf[buf_offset];
				off++;
				len--;
				copied++;
			}	
		}
		 return copied;
	 }
							
	 public char peek() throws IOException {

		 while(true) {

			 if (buf_left == 0) {
				 // copy more into buffer	
				 buf_offset = 0;
				 buf_left = in.read(buf, 0, buf.length);
			 }
			 if (buf_left == -1)
				 return 0;
			 
			 // for each char in the buffer
			 for (; buf_left > 0; buf_left --, buf_offset++){
				 boolean token = false;
				 // compare with white
				 for (int j = 0; j < white.length; j++) {
					 if (buf[buf_offset] == white[j]){
						 token = true;
						 break;
					 }
				 }
				 // not a token
				 if (!token)
					 return (char)buf[buf_offset];		
			 }	
		 }	
	 }

	public void eatWhite() throws IOException{

		while(true) {

			if (buf_left == 0) {
				 // copy more into buffer	
				buf_offset = 0;
				buf_left = in.read(buf, 0, buf.length);
			}
			if (buf_left == -1)
				return;
			
			 // for each char in the buffer
			for (; buf_left > 0; buf_left --, buf_offset++){
				// compare with white
				if (buf[buf_offset] == ' ' ||
					buf[buf_offset] == '\t' ||		 
					buf[buf_offset] == '\r' ||		 
					buf[buf_offset] == '\n'){
						 continue;
				} else
					return;
			}
		}	
	}	
}



		 




