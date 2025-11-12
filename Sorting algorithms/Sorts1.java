import java.util.*;

public class Sorts1
{
   public static <T extends Comparable<? super T>> void heapSort(T[] array) 
   {
       int length = array.length;

       for (int i = length / 2 - 1; i >= 0; i--) //build method
       {
           heapify(array, length, i);
       }

       for (int i = length - 1; i > 0; i--) //(swap method) 
       {
           T temp = array[0];
           array[0] = array[i]; //length -1
           array[i] = temp;

           heapify(array, i, 0);
       }
    }

   private static <T extends Comparable<? super T>> void heapify(T[] array, int length, int i) 
   {
       int left = 2 * i + 1; //left child
       int right = 2 * i + 2; //right child
       int large = i;       
    
    
       if (left < length && array[left].compareTo(array[large]) > 0) 
       {
           large = left;
       }
   
       if (right < length && array[right].compareTo(array[large]) > 0) 
       {
           large = right;
       }

       if (largest != i) 
       {
           T temp = array[i];
           array[i] = array[large];
           array[large] = temp;

           heapify(array, length, large);
       }
   }   
   
   
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