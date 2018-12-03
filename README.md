# munkres 
A Pure C version of Munkres' Assignment Algorithm (Hungarian Algorithm) <br>
The PDF explains the algorithm step by step and the print of program corresponds to the explanation.  <br>
The Repository should be greatly helpful to understand the Munkres' Assignment Algorithm listed in this webpage: http://csclab.murraystate.edu/~bob.pilgrim/445/munkres.html
<p>
munkres.c : C implement of Munkres' Assignment Algorithm <br>
munkres.h : Header for munkres.c <br>
py_wrapper.c : The wrapper for Python3 <br>
<p>
Munkres.pdf : PDF which details the algorithm <br>
Installation.pdf : PDF which details the installation <br>

<p><br>
Here is a way to compile the munkres.so without Eclipse: <br>
Prerequisite: $ sudo apt install g++ cmake python3-dev python3-numpy (tested in Ubuntu18.04.1) <br>
$ gcc -shared -fPIC -o munkres.so munkres.c py_wrapper.c -I /usr/include/python3.6 -include munkres.h -l python3.6 -L /usr/lib/python3.6/config-3.6m-x86_64-linux-gnu/ <br>
