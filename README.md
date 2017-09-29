# Signal

So this is a program that takes in a .GIF file and spits it back out in its input stream form. It spits out its output into a file called data.txt, which should be provided in the folder where the program is run.

There are some problems with this program (mainly that it begins to spout out sketchy output once there are multiple image sub-blocks). This means that there are no problems with working with smaller images, but the larger the image gets, the higher the chance is that there will be a messup.

I know the problem lies with the getImageData method, and most likely it's with the while loop within it and how I handle codeSize and reinitializing the codeTable.

This was made with the assistance of the URL below:
http://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
