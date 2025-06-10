# f_cube

A binary visualizer project based on initial concepts presented by Chris Domas at Derbycon in 2012. The idea is that if you look at binary files as a series of (X,Y,Z) coordinates and map their statistical frequency to brightness certain patterns begin to emerge in 2D and 3D. Astonishingly, the 3D patterns that you can pull out from a file based on a simple trigram frequency analysis not only aren't random noise (well, sometimes they are) but create shapes that seem to isomorphically map a visual space onto the non-visual taxonomic file classifications.

In simpler words, all text files seem to have a similar structure. .wav files seem to have their own specific type of shape. Instructions (executable and object code) seem to have their own specific type of a shape with specific differences noticeable even between code compiled for x86_64 and code compiled for PowerPC. Not only can you look at shapes of the entire file but you can imagine being able to break the file into smaller parts interactively and visually carve a file into separate sub-modules just based on the kind of shape they have.

Several attempts have been made to implement this in a form accessible as an open-source program. They range from an exercise to do during a streaming session to very impressive tools. I have made an attempt to list all the prior work below, including papers, videos, and code.

The best attempt (in my mind and to my needs) that I have identified so far has been [Veles](https://github.com/codilime/veles). It is written in C++ using Qt/OpenGL with specific bits seemingly offloaded onto graphic shaders. It has implemented 2D and 3D visualization with the ability to break the file into slices visually. I have actually used Veles several times with quite productive results.

Unfortunately, Veles is not perfect. The author archived the project in 2020 so we aren't particularly expecting any updates. It is written with the use of OpenGL which has been deprecated. And most importantly, for reasons which I'm sure made a lot of sense to the author at the time the project seems to employ a kind of client<->server architecture that is very much mandatory and you are required to start a server and connect to it in order to do anything in the application. I must emphasize that my main suspect for why this doesn't make sense to me is simply that I am not that good. Having said that, that is even more of a reason to try another crack at this.

Ironically, my first prototype is also disjointed. There is a python script that you aim at the file in question, hit enter and wait for trigrams.json to pop out in $(cwd). Then you must launch the TrigramVoxelViewer with --inputfile as its argument.  This last part is written with C++17 and Vulkan because I want to be able to do on this on a laptop that's not equipped with the latest 4090 and also I just know C++.

The build process is very simple:

take build # or mkdir build && cd build
cmake ..
make
That's it. Now you should have whatever I named the binary in build scripts in fcube/build.

For now this is a prototype that I want to upload because I finally got basic voxel stuff ported to Vulkan (Vulkan likes lots of code! damn). Hopefully very soon i can come back to this and the first order would be to port Python code into the C++ side so we don't have this strange process of prep & visualization in two separate places.

**References**

*video* [REcon 2013 - The Future of RE : Dynamic Binary Visualization (Christopher Domas)](https://www.youtube.com/watch?v=C8--cXwuuFQ)

*code* [Veles](https://github.com/codilime/veles)

*code* [tsoding/binviz](https://github.com/tsoding/binviz)

*code* [kentavv/binary_viewer](https://github.com/kentavv/binary_viewer)

*code* [wapiflapi/binglide](https://github.com/wapiflapi/binglide)

*code* [rebelbot/binvis](https://github.com/rebelbot/binvis)

*pdf* [A Visual Study of Primitive Binary Fragment Types by Conti](https://media.blackhat.com/bh-us-10/whitepapers/Bratus_Conti/BlackHat-USA-2010-Bratus-Conti-Taxonomy-wp.pdf)

*slides* [Dmk Blackops 2006 by Dan Kaminski of all people](https://www.slideshare.net/slideshow/dmk-blackops2006/5939088)
