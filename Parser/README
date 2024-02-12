- parser.cpp and parser.h

  These are skeleton files. parser.cpp does not do any parsing. Instead
  it calls a function that reads and prints all tokens in the standard
  input. It is provided as an illustration of how to use the GetToken() 
  function.

  To see how this works, compile the files in this directory using the
  command line:

	g++ -std=c++11 -Wall *.cpp

  This will compile all the .cpp files and generates an executable a.out in
  the same directory. 

  Now, execute the following

	./a.out <test1.txt

  This will print all the tokens that appear in test1.txt. Recall, as explained
  in the project description document towards the end, that using <test1.txt
  directs the shell to treat test1.txt as the standard input. 

- test1.sh 

  this file contains a script that allows you to test your program on 
  multiple test cases with one command. The file test1.sh should be 
  in a directory together with the executable a.out and a test cases
  directory called provided_tests. The directory provided_tests has 
  subdirectories for various categories of test cases. In each directory
  there is a number of test cases (test.txt files) and a corresponding 
  number of expected outputs (test.txt.expected files) for each of the 
  test cases. test1.sh will execute a.out on each of the test.txt files
  and compares the output that it produces to the output specified in the 
  test.txt.expected file. If the produced output matches the expected 
  output, the test cases passes, otherwise it fails.

  As you develop your solution, you do not need to execute a.out manually
  on each test case. Instead, you can execute test1.sh as follows

	./test1.sh
  
  and it will do all the work of executing your program against each
  test cases.

  Note: if you are not able to execute ./test1.sh and get a complaint that
  the file is not executable, execute the following on the command line

	chmod +x test1.sh

- lexer.cpp and lexer.h

  These two files include the implementation of the GetToken() and Peek() 
  methods that I have already covered in class. You do not need to read 
  lexer.cpp or understand how it work in order for you to use it. 

  All you need to know are the functions made available by these files. Their 
  use is illustrated in parser.cpp

- inputbuf.cpp and inputbuf.h

  These two files provide functionality that is needed to implement the 
  GetToken() method in lexer.cpp. You can safely ignore these two files 
  completely.
