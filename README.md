# Solving Overlapping Coalition Structure Generation in Task-Based Settings
The test instances and codes for the manuscript submitted to Journal of Artificial Intelligence Research.

# (Standard C for Unix/linux/Windows or any other platform) How to Compile Our .c File 
Please note: all the .c files are located in the Linux directory.
1. Make sure that the random-gcc.h file and the there .c files are in the same directory.

2. Then, you can compile the code (e.g., BPSO-OCSG-gcc.c) using gcc:
   
   gcc BPSO-OCSG-gcc.c -o bpso -lm       /* bpso is the name of the output file */

4. After that, run the generated executable:
   
   ./bpso          /* on Linux or macOS */

   or 

      bpso.exe        /* on Windows (using MinGW or Cygwin)*/

# (C++ for Windows) How to Compile Our .cpp File Using Visual Studio 2013
Please note: all the .cpp files are located in the Window directory.
1.Open Visual Studio 2013

Launch Visual Studio 2013 from your Start menu or desktop shortcut.

2.Create a New Project

Go to File > New > Project.

In the New Project window, select Visual C++ under Templates.

Choose Win32 Console Application.

Enter a name (e.g., BPSO-OCSG) for your project and specify the location where you want to save it.

Click OK.

3.Set Up the Project

In the Win32 Application Wizard, click Next.

Select Console Application under Application type.

Check Empty project under Additional options.

Click Finish.

4.Add random.h File in the Project

In the Solution Explorer (usually on the right side), right-click on the Source Files folder.

Select Add > New Item.

In the Add New Item window, choose the random.h File.

5.Paste the Code

Open the newly created .cpp (whose name is the same as the project name) file in the editor.

Paste the C++ code of the shared BPSO-OCSG.cpp, DE-OCSG.cpp, or GA-OCSG.cpp into the file.

Please note: You can only paste one .cpp file at a time, corresponding to one algorithm.

6.Compile the Code

Go to Build > Build Solution (or press F7).

Visual Studio will compile your code. 

7.Run the Program

Once the code compiles successfully, go to Debug > Start Without Debugging (or press Ctrl + F5).

A console window will open, and you will see the output of your program.

# Additional Notes:
The program will output three txt files in the current directory where the project was created:

1.tim_suite*.txt: This file saves the running time of the algorithm.

2.val_suite*.txt: This file saves the best overlapping coalition structure value obtained by the algorithm.

3.enc_suite*.txt: This file saves the best solution (i.e., the best overlapping coalition structure for the given tasks) obtained by the algorithm.

# How to Use Our Test Instance File
1.Every suite*.txt file represents an instance, including:

int Resource_Agent[maxagent][maxresource] = { };

int Resource_Goal[maxgoal][maxresource] = { };

int Reward_Goal[maxgoal] = { };

int Interest_Set[maxagent][maxgoal] = { };

2.When you select an instance, please update the following information in the .cpp file:

//#define maxagent   ? /*Max no. of agents */

//#define maxgoal    ? /*Max no. of goals */

//#define maxresource    ?/*Max no. of resources */

//#define no_suite  ?/*index for instance */

//int Resource_Agent[maxagent][maxresource] = { };

//int Resource_Goal[maxgoal][maxresource] = { };

//int Reward_Goal[maxgoal] = { };

//int Interest_Set[maxagent][maxgoal] = { };

3.Then, you can compile the code to test this instance.
