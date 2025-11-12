import java.util.*;
import java.io.File;
import java.io.FileNotFoundException;


public class Main
{
   public static int [] start; 
   public static char [][] maze;
      
   public static void scanMaze() throws FileNotFoundException
   {
     Scanner scan = new Scanner(new File("maze.dat"));
     int rows = scan.nextInt();
     int cols = scan.nextInt(); 
     scan.nextLine();
     
     maze = new char [rows][cols];
     for(int r = 0; r < rows; r++)
     {
        String lineScan = scan.nextLine();
        if (lineScan.length() != cols) 
        {
           System.out.println("could not solve maze");
           return;
        }
        for (int c = 0; c < cols; c++)
        {
           maze[r][c] = lineScan.charAt(c);
        }
     }
   }
    
   public static int[] findStart(char maze [][])
   {
      for(int r =0; r < maze.length; r++) 
      {
         for(int c = 0; c < maze[0].length; c++)
         {
            if (maze[r][c] == '+')
            {
               start = new int[2]; // return new int[] {r,c};
               start[0] = r;
               start[1] = c;
               return start;
            }
         }
      } 
      return null;  
   }
   
   public static boolean checkSpot(char maze[][],int row, int col)
   {
     return (row >= 0 && row <= maze.length && col >= 0 && col <= maze[0].length && maze[row][col] == ' ');  
   }
   
   public static boolean move(char maze[][], int r, int c)
   {
      if (r < 0 || r >= maze.length || c < 0 || c >= maze[0].length) 
      {
         return false;
      }
      if (maze[r][c] == '-') 
      {
         return true;
      }
      
      if (maze[r][c] != ' ') 
      {
         return false;
      }
      
      maze[r][c] = '+';
      
      if (move(maze,r +1,c) || move(maze,r-1,c) || move(maze,r,c+1) || move(maze,r,c-1))
      {
         return true;
      }
      maze[r][c] = '.';
      return false;
   }
   
   public static void main (String args [])
   {
      try
      { 
         scanMaze();
         start = findStart(maze);
         if (start != null)
         {
           if(move(maze,start[0],start[1]+1)) //so move scans the next spot  
           {
             System.out.println("The maze had been solved");
           }
           else if (move(maze,start[0],start[1]-1)) //if start is on right side
           {
             System.out.println("The maze had been solved");
           }
           else
           {
             System.out.println("Can not solve maze, something is blocking the exit");
           }
         }
         else
         {
            System.out.println("No start point found, cant solve maze");
         }
         for (char[] row : maze) 
         {
            System.out.println(new String(row));
         } 
      }
      catch (FileNotFoundException e)
      {
         System.out.println("Could not solve maze, no file found");
      }

   }
}

