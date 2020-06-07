import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import javax.imageio.ImageIO;
public class pngtoascii
{
	public static void main(String[] args) throws IOException
	{
		if (args.length == 0)
		{
			return;
		}
		BufferedImage img = ImageIO.read(new File(args[0]));
		Raster pixels = img.getData();
		BufferedOutputStream w = new BufferedOutputStream(new FileOutputStream("charmap"));
		
		int[] dat = new int[3];
		int bitbuffer = 0;
		int bitindex = 0;
		for (int chr = 0;chr<128;chr++)
		{
			for (int x = 0;x<10;x++)
			{
				for (int y = 0;y<10;y++)
				{
					dat = pixels.getPixel(y+10*chr,x,dat);
					if (dat[0] == 0 && dat[1] == 0 && dat[2] == 0)
					{
						bitbuffer = bitbuffer | (1 << bitindex);
						bitindex++;
						if (bitindex == 8)
						{
							bitindex = 0;
							w.write(bitbuffer);
							bitbuffer = 0;
						}
					}
					else
					{
						bitindex++;
						if (bitindex == 8)
						{
							bitindex = 0;
							w.write(bitbuffer);
							bitbuffer = 0;
						}
					}
				}
			}
		}
		w.close();
	}
}