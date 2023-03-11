
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.net.Socket;
import java.rmi.UnknownHostException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.KeyAgreement;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.DHParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;

public class Protocol2Attack {

    
    static BigInteger g = new BigInteger("129115595377796797872260754286990587373919932143310995152019820961988539107450691898237693336192317366206087177510922095217647062219921553183876476232430921888985287191036474977937325461650715797148343570627272553218190796724095304058885497484176448065844273193302032730583977829212948191249234100369155852168");
    static BigInteger p = new BigInteger("165599299559711461271372014575825561168377583182463070194199862059444967049140626852928438236366187571526887969259319366449971919367665844413099962594758448603310339244779450534926105586093307455534702963575018551055314397497631095446414992955062052587163874172731570053362641344616087601787442281135614434639");

    static Cipher decAESsessionCipher;
    static Cipher encAESsessionCipher;
    
    public static void main(String[] args){
    
        String hostname = "localhost";
        
        Socket server = null;
        DataOutputStream os = null;
        DataInputStream is = null;
        
        Socket server2 = null;
        DataOutputStream os2 = null;
        DataInputStream is2 = null;
        
        
        try{
            server = new Socket(hostname, 11338 );
            os = new DataOutputStream (server.getOutputStream());
            is = new DataInputStream(server.getInputStream());
            
            // create two seperate sessions so the server can encrypt Ns+1 for us
            server2 = new Socket(hostname, 11338 );
            os2 = new DataOutputStream (server2.getOutputStream());
            is2 = new DataInputStream(server2.getInputStream());
            
            // use crypto API for Diffie Hellman
            DHParameterSpec dhSpec = new DHParameterSpec(p,g);
            KeyPairGenerator diffieHellmanGen = KeyPairGenerator.getInstance("DiffieHellman");
            diffieHellmanGen.initialize(dhSpec);
            KeyPair clientPair = diffieHellmanGen.generateKeyPair();
            PrivateKey x = clientPair.getPrivate();
            PublicKey gToTheX = clientPair.getPublic();
            
            // send the server our gToTheX
            os.writeInt(gToTheX.getEncoded().length);
            os.write(gToTheX.getEncoded());
            
            // receive the servers gToTheY
            int publicKeyLen = is.readInt();
            byte[] message1 = new byte[publicKeyLen];
            is.read(message1);
            KeyFactory keyFactoryDH = KeyFactory.getInstance("DH");
            X509EncodedKeySpec x509Spec = new X509EncodedKeySpec(message1);
            PublicKey gToTheY = keyFactoryDH.generatePublic(x509Spec);
            
            calculateSessionKey(x, gToTheY);
            
            // gen a random nonce (don't really need to do this, could just be 0's )
            SecureRandom gen = new SecureRandom();
            int clientNonce = gen.nextInt();
            byte[] clientNonceBytes = BigInteger.valueOf(clientNonce).toByteArray();
            
            // Encrypt and send our nonce as message 3
            byte[] message3enc = encAESsessionCipher.doFinal(clientNonceBytes);
            os.write(message3enc);
            os.flush();
            
            // receive and decrypt message 4 (the servers reply)
            byte[] message4enc = new byte[32];
            is.read(message4enc);
            byte[] message4 = decAESsessionCipher.doFinal(message4enc);
            
            // get the servers nonce
            byte[] serverNonceBytes = new byte[4];
            System.arraycopy(message4, 16, serverNonceBytes, 0, 4);
            int serverNonce = new BigInteger(serverNonceBytes).intValue();
            System.out.println("Server Nonce = " + serverNonce);
            
            // Save session Ciphers:
            Cipher decOriginalCipher = decAESsessionCipher;
            Cipher encOriginalCipher = encAESsessionCipher;
            
            // switch to second connection/session
            
            // send our gToTheX again
            os2.writeInt(gToTheX.getEncoded().length);
            os2.write(gToTheX.getEncoded());
            
            // get second gToTheY
            int publicKeyLen2 = is2.readInt();
            byte[] message1_2 = new byte[publicKeyLen2];
            is2.read(message1_2);
            KeyFactory keyFactoryDH2 = KeyFactory.getInstance("DH");
            X509EncodedKeySpec x509Spec2 = new X509EncodedKeySpec(message1_2);
            PublicKey gToTheY2 = keyFactoryDH2.generatePublic(x509Spec2);
            
            // recalculate session ciphers for second socket/connection
            calculateSessionKey(x, gToTheY2);
            
            // send server nonce as 'our' nonce
            byte[] message3enc2 = encAESsessionCipher.doFinal(serverNonceBytes);
            os2.write(message3enc2);
            os2.flush();
            
            // receive and decrypt server's reply
            byte[] message4enc2 = new byte[32];
            is2.read(message4enc2);
            byte[] message4_2 = decAESsessionCipher.doFinal(message4enc2);
            
            // get the ecrypted Ns + 1 that we need for the original connection
            byte[] encryptedServerNonceInc = new byte[16];
            System.arraycopy(message4_2, 0, encryptedServerNonceInc, 0, 16);
            
            // encrypt and send message 5 (the encrypted Ns + 1) to the original connection
            byte[] message5enc = encOriginalCipher.doFinal(encryptedServerNonceInc);
            os.write(message5enc);
            os.flush();
            
            // receive final message
            byte[] message6enc = new byte[432];
            int len3 = is.read(message6enc);
            // use line below to work out that the length is 432
            // with the above originally set to: message6enc = new byte[1024]
            System.out.println("message 6 enc len = " + len3);
            
            // decrypt and print
            byte[] message6 = decOriginalCipher.doFinal(message6enc);
            System.out.println(new String(message6));
            
        } catch (UnknownHostException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (java.net.UnknownHostException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (InvalidAlgorithmParameterException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (InvalidKeySpecException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } catch (BadPaddingException e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        } 
    }
        
    //**// taken from Protocol2Server.java
    // This method sets decAESsessioncipher & encAESsessioncipher 
    static void calculateSessionKey(PrivateKey x, PublicKey gToTheY)  {
        try {
        // Find g^xy
        KeyAgreement serverKeyAgree = KeyAgreement.getInstance("DiffieHellman");
        serverKeyAgree.init(x);
        serverKeyAgree.doPhase(gToTheY, true);
        byte[] secretDH = serverKeyAgree.generateSecret();
        //Use first 16 bytes of g^xy to make an AES key
        byte[] aesSecret = new byte[16];
        System.arraycopy(secretDH,0,aesSecret,0,16);
        Key aesSessionKey = new SecretKeySpec(aesSecret, "AES");
        // Set up Cipher Objects
        decAESsessionCipher = Cipher.getInstance("AES");
        decAESsessionCipher.init(Cipher.DECRYPT_MODE, aesSessionKey);
        encAESsessionCipher = Cipher.getInstance("AES");
        encAESsessionCipher.init(Cipher.ENCRYPT_MODE, aesSessionKey);
        } catch (NoSuchAlgorithmException e ) {
        System.out.println(e);
        } catch (InvalidKeyException e) {
        System.out.println(e);
        } catch (NoSuchPaddingException e) {
        e.printStackTrace();
        }
    }
    
}
