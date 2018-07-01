#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

enum { READ, WRITE };

int main(int argc, char ** argv)
{
  pid_t p_child;
  pid_t p_grandchild;
  pid_t p_ggrandchild;
  int file_descriptor1[2];
  int file_descriptor2[2];
  int zero = 0;
  int status;

  // Creates pipe 1
  int rc = pipe(file_descriptor1);
  if(rc < zero)
    {
      perror("Error: Pipe 1 failed");
      cout << endl;
      exit(EXIT_FAILURE);
    }

  // Fork child process
  p_child = fork();
  if (p_child < zero)
    {
      perror("Error: Fork of parent process failed");
      cout << endl;
      exit(EXIT_FAILURE);
    }
  else if(p_child == zero)
    {

      // Create pipe 2
      if(pipe(file_descriptor2) < 0)
	{
	  perror("Error: Second pipe failed");
	  cout << endl;
	  exit(EXIT_FAILURE);
	}

      // Create grandchild
      p_grandchild = fork();
      
      if(p_grandchild < zero)
	{
	  perror("Error: Fork of child process failed");
	  cout << endl;
	  exit(EXIT_FAILURE);
	}
      else if(p_grandchild == zero)
	{
	  // Create great-grandchild
	  p_ggrandchild = fork();
	  if(p_ggrandchild < zero)
	    {
	      perror("Error: Fork of grandchild process failed");
	      cout << endl;
	      exit(EXIT_FAILURE);
	    }
	  else if(p_ggrandchild == zero)
	    {
	      // Run great-grandchild command: " ps -A"
	      close(file_descriptor1[READ]); 
	      close(file_descriptor2[WRITE]);
	      rc = dup2(file_descriptor1[WRITE], WRITE);
	      if(rc < zero)
	      {
	      	perror("Error: Dup of file_descriptor write 1 failed");
	      	cout << endl;
	      	exit(EXIT_FAILURE);
	      }
	      execlp("ps", "ps", "-A", NULL);
	    }
	  else
	    {
	      // Run grandchild command "grep argv[1]"
	      close(file_descriptor1[WRITE]);
	      close(file_descriptor2[READ]);
	      rc = dup2(file_descriptor1[READ], READ);
	      if(rc < zero)
	      {
	      	perror("Error: Dup of file_descriptor 1 read failed");
	      	cout << endl;
	      	exit(EXIT_FAILURE);
	      }
	      rc = dup2(file_descriptor2[WRITE], WRITE);
	      if(rc < zero)
	      {
	      	perror("Error: Dup of file_descriptor 2 Write failed");
	      	cout << endl;
	      	exit(EXIT_FAILURE);
	      }
	      execlp("grep", "grep", argv[1], NULL);
	    }
	}
      else
	{
	  // Run child command: "wc -l"
	  close(file_descriptor1[READ]);
	  close(file_descriptor1[WRITE]);
	  close(file_descriptor2[WRITE]);
	  rc = dup2(file_descriptor2[READ], READ);
	  if(rc < zero)
	      {
	      	perror("Error: Dup of file_descriptor 2 read failed");
	      	cout << endl;
	      	exit(EXIT_FAILURE);
	      }
	  execlp("wc", "wc", "-l", NULL);
	}
    }
  else
    {
      // Parent process
      // Hygiene: Closing all file_descriptors
      for(int i = file_descriptor1[READ]; i < file_descriptor2[WRITE]; i++)
	{
	  close(i);
	}
      // Wait for child processe(s) to finish
      wait(&status);
    }
  
  return 0;
}
