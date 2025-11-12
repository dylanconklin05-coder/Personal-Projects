import java.util.*;
import java.util.Random;

public class MainP3 
{
   public static void main(String[] args) {
        int size = 50;

        
        Integer[] arr = new Integer[size];
        Random rand = new Random();
        for (int i = 0; i < size; i++) 
        {
            arr[i] = rand.nextInt(100); 
        }

        long startTime = System.nanoTime();
        heapSort(arr);
        long endTime = System.nanoTime();
        long duration = endTime - startTime;
        double durationInSeconds = duration / 1_000_000_000.0;
        System.out.println("Time taken to sort " + size + " elements: " + durationInSeconds + " seconds.");
        for (int i = 0; i < 50 && i < size; i++) {
            System.out.print(arr[i] + " ");
        }
        System.out.println("...");
    }
}