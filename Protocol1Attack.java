
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;
import java.io.InputStream;

public class Protocol1Attack {

	
	static String ipAdd = "127.0.0.1";
	static int portNo = 11337;
	static boolean debug = true;

	public static void main(String[] args) throws IOException, InvalidKeyException, NoSuchAlgorithmException,
			NoSuchPaddingException, IllegalBlockSizeException, BadPaddingException {
		if(debug)
		    System.out.println("open socket");
		Socket socket = new Socket(ipAdd, portNo);
		OutputStream outStream = socket.getOutputStream();
		InputStream inStream = socket.getInputStream();

		// Step 1
		// Send "Connect Protocol 1" to server in ASCII
		if(debug)
		
		outStream.write("Connect Protocol 1".getBytes());
		System.out.println("Message sent");



		// Step 2
		//Receive nonce from server
		byte[] nonce = new byte[32];
		inStream.read(nonce);
		if(debug)
		    System.out.println("Encrypted Nonce is: " + byteArrayToHexString(nonce));

		// Step 3
		// Return nonce to server
		if(debug)
		System.out.println("Client nonce is " + byteArrayToHexString(nonce));
		outStream.write(nonce);
		
		byte[] key = new byte[16];
		SecretKeySpec secretKeySpec = new SecretKeySpec(key, "AES");
		Cipher decAEScipherSession = Cipher.getInstance("AES");
		decAEScipherSession.init(Cipher.DECRYPT_MODE, secretKeySpec);
		Cipher encAEScipherSession = Cipher.getInstance("AES");
		encAEScipherSession.init(Cipher.ENCRYPT_MODE, secretKeySpec);
		if(debug)
		    System.out.println("Session key is "+byteArrayToHexString(key));
		

		
		// Step 4
		// receive two identical nonces from server
		byte[] message2 = new byte[48];
		inStream.read(message2);
 		if (debug)
		    System.out.println("Encrypted nonces are " +byteArrayToHexString(message2));

		//Step 5
		// send the same message back to the server
  		outStream.write(message2);



		//Step 6
		//Read secret input from server
		 byte[] secretInput = new byte[inStream.available()];
		 inStream.read(secretInput);
		 byte[] secret = decAEScipherSession.doFinal(secretInput);
		 if(debug) 
			System.out.println("Secret Input : " + new String(secret));
	 	socket.close();
	}



        private static byte[] xorBytes (byte[] one, byte[] two) {
	if (one.length!=two.length) {
	    return null;
	} else {
	    byte[] r = new byte[one.length];
	    for(int i=0;i<one.length;i++) {
		r[i] = (byte) (one[i]^two[i]);
	    }
	    return r;
	}
    }
    
    private static String byteArrayToHexString(byte[] d) { 
	StringBuffer buffer = new StringBuffer();
	for (int i = 0; i < d.length; i++) { 
	    int halfbyte = (d[i] >>> 4) & 0x0F;
	    int two_halfs = 0;
	    do { 
		if ((0 <= halfbyte) && (halfbyte <= 9)) 
		    buffer.append((char) ('0' + halfbyte));
		else 
		    buffer.append((char) ('a' + (halfbyte - 10)));
		halfbyte = d[i] & 0x0F;
	    } while(two_halfs++ < 1);
	} 
	return buffer.toString();
    } 
    
    private static byte[] hexStringToByteArray(String s) {
	int len = s.length();
	byte[] d = new byte[len / 2];
	for (int i = 0; i < len; i += 2) {
	    d[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
				  + Character.digit(s.charAt(i+1), 16));
	}
	return d;
    }


}


		

