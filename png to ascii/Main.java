import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import javax.imageio.ImageIO;
public class Main
{
	public static void main(String[] args) throws IOException
	{
		System.out.println("enter image path: ");
		Scanner s = new Scanner(System.in);
		BufferedImage img = ImageIO.read(new File(s.nextLine()));
		s.close();
		Raster pixels = img.getData();
		BufferedWriter w = new BufferedWriter(new FileWriter("ascii.c"));
		w.write("char ascii10p[128][10][10] = {");
		int dat[] = null;
		for (int chr = 0;chr<128;chr++)
		{
			w.write("{");
			for (int x = 0;x<10;x++)
			{
				w.write("{");
				StringBuilder build = new StringBuilder();
				for (int y = 0;y<10;y++)
				{
					dat = pixels.getPixel(y+10*chr,x,dat);
					if (dat[0] == 0 && dat[1] == 0 && dat[2] == 0)
					{
						build.append(1);
					}
					else
					{
						build.append(0);
					}
					build.append(",");
				}
				build.deleteCharAt(build.length()-1);
				w.write(build.toString());
				w.write("}");
				if (x != 9)
					w.write(",");
			}
			w.write("}");
			if (chr != 127)
				w.write(",");
		}
		
		w.write("};");
		w.close();
	}
}