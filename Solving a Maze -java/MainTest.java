import java.util.*;
import java.io.File;
import java.io.FileNotFoundException;

public class MainTest {
    public static char[][] maze;

    public static void scanMaze() throws FileNotFoundException {
        Scanner scan = new Scanner(new File("mazeTest.dat"));
        int rows = scan.nextInt();
        int cols = scan.nextInt();
        scan.nextLine();

        maze = new char[rows][cols];
        for (int r = 0; r < rows; r++) {
            String lineScan = scan.nextLine();
            if (lineScan.length() != cols) {
                System.out.println("Could not solve maze: Row length mismatch");
                return;
            }
            for (int c = 0; c < cols; c++) {
                maze[r][c] = lineScan.charAt(c);
            }
        }
    }

    public static int[] findStart() {
        for (int r = 0; r < maze.length; r++) {
            for (int c = 0; c < maze[0].length; c++) {
                if (maze[r][c] == '+') {
                    return new int[]{r, c};
                }
            }
        }
        return null;  
    }

    public static boolean checkSpot(int row, int col) {
        return (row >= 0 && row < maze.length && col >= 0 && col < maze[0].length &&
                (maze[row][col] == ' ' || maze[row][col] == '-'));
    }

    public static boolean move(int r, int c) {
        // Print current position
        System.out.println("Trying to move to (" + r + ", " + c + ")");

        // Check if out of bounds or not a valid space
        if (!checkSpot(r, c)) {
            System.out.println("Invalid move at (" + r + ", " + c + ")");
            return false;
        }

        // If we found the end, return true
        if (maze[r][c] == '-') {
            maze[r][c] = '+'; // Mark the end as part of the path
            System.out.println("Reached the end at (" + r + ", " + c + ")");
            return true;
        }

        // Mark the current position as part of the path
        maze[r][c] = '+';
        System.out.println("Marked path at (" + r + ", " + c + ")");

        // Explore all four directions
        if (move(r + 1, c) || move(r - 1, c) || move(r, c + 1) || move(r, c - 1)) {
            return true; // If any direction leads to a solution
        }

        // If none of the directions worked, mark it as a dead end
        maze[r][c] = '.';
        System.out.println("Dead end at (" + r + ", " + c + ")");
        return false; // No path found
    }

    public static void main(String[] args) {
        try {
            scanMaze();
            int[] startSpot = findStart();
            if (startSpot != null) {
                move(startSpot[0], startSpot[1]);
            }

            // Print the completed maze
            for (char[] row : maze) {
                System.out.println(new String(row));
            }
            System.out.println("Maze is complete");
        } catch (FileNotFoundException e) {
            System.out.println("Could not solve maze: " + e.getMessage());
        }
    }
}
