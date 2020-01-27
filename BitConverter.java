package iag0010javaplantlogger;

import java.io.*;
import java.nio.*;

public class BitConverter {
    
    /*
        Extract 32-bit integer from byte array
    */
    static public int ToInt32(byte Buf[], int offset) throws ArrayIndexOutOfBoundsException {
        int i, Result;
        try {
            for (i = Result = 0;  i < 4;  i++)
                Result |= ((0xFF & ((int)Buf[offset + i]))) << (i * 8);
        }
        catch (ArrayIndexOutOfBoundsException aioobe) {
            throw aioobe;
        }
        return Result;
     }
    
    /*
        Extract 64-bit integer from byte array
    */
     static public long ToInt64(byte Buf[], int offset) throws ArrayIndexOutOfBoundsException {
        int i, Result;
        try {
            for (i = Result = 0;  i < 8;  i++)
                Result |= ((0xFF & ((int)Buf[offset + i]))) << (i * 8);
        }
        catch (ArrayIndexOutOfBoundsException aioobe) {
            throw aioobe;
        }
        return Result;
     }
    
    /*
        Extract double from byte array
    */
    static public double ToDouble(byte Buf[], int offset) throws ArrayIndexOutOfBoundsException {
        try {
            //return Double.longBitsToDouble(ToInt64(Buf, offset));
            return ByteBuffer.wrap(Buf, offset, 8).order(ByteOrder.LITTLE_ENDIAN).getDouble();

        }
        catch (ArrayIndexOutOfBoundsException aioobe) {
            throw aioobe;
         }
    }
    
    /*
        Extract string from byte array. The encoding may be UTF-16LE (LE - little endian) or US_ASCII (character 0... 127)
    */
    static public String ToString(byte Buf[], int offset, String encoding) throws ArrayIndexOutOfBoundsException, UnsupportedEncodingException{
        int i;
        if (encoding.equals("UTF-16LE")) {
            try {
                for (i = offset;  true;  i += 2) {
                    if (Buf[i] == 0 && Buf[i + 1] == 0) {
                        break;
                    }
                }
                return new String(Buf, offset, i - offset, encoding); 
            }
            catch (ArrayIndexOutOfBoundsException aioobe) {
                throw aioobe;
            }
        }
        else if (encoding.equals("US_ASCII")) {
            try {
                for (i = offset;  true;  i++) {
                    if (Buf[i] == 0) {
                        break;
                    }
                }
                return new String(Buf, offset, i - offset);
            }
            catch (ArrayIndexOutOfBoundsException aioobe) {
                throw aioobe;
            }
        } 
        else {
            throw new UnsupportedEncodingException();
        }
    }
    
    /*
        Inserts 32-bit integer into byte array
    */
    static public void ToBytes(int n, byte[] Buf, int offset) throws ArrayIndexOutOfBoundsException {
         try {
            for (int i = 0;  i < 4;  i++) {
               Buf[offset + i] = (byte)(n & 0xFF);
               n >>= 8;
            }
        }
        catch (ArrayIndexOutOfBoundsException aioobe) {
              throw aioobe;
        }
    }
     
    /*
       Inserts string into byte array
    */
    static public void ToBytes(String str, byte[] Buf, int offset) throws ArrayIndexOutOfBoundsException {
        int i, j;
        short s;
        try {
            for (i = j = 0;  j < str.length();  j++, i+=2) {
                s = (short)str.charAt(j);
                Buf[i + offset] = (byte)(s & 0xFF);
                Buf[i + offset + 1] = (byte)(s >> 8);
            }
            Buf[i + offset] = Buf[i + offset + 1] = 0;
        }
        catch (ArrayIndexOutOfBoundsException aioobe) {
            throw aioobe;
        }
    }
}
