import java.util.*;

public class MainP2 {
public static void main(String[] args) {
Random rand = new Random(1);
List<Integer> list = new SortedList<Integer>();
int n, num;
System.out.println("insert");
long start = System.currentTimeMillis();
for (int i = 0; i < (num = args.length == 1 ? Integer.parseInt(args[0]) :
11); ++i) {
n = rand.nextInt(num);
list.insert(n);
System.out.print(n + ": ");
for (Integer j : list) {
System.out.print(j + " ");
}
System.out.println();
}
long stop = System.currentTimeMillis();
System.out.println(stop - start);
System.out.println("search");
for (int i = 0; i < num / 2; ++i) {
n = rand.nextInt(num);
System.out.println(n + ": " + list.search(n));
}
System.out.println("retrieve");
for (int i = -1; i < num+1; ++i) {
System.out.println(i + ": " + list.retrieve(i));
}
// rand = new Random(1);
System.out.println("remove");
start = System.currentTimeMillis();
for (int i = 0; i < num; ++i) {
n = rand.nextInt(num);
list.remove(n);
System.out.print(n + ": ");
for (Integer j : list) {
System.out.print(j + " ");
}
System.out.println();
}
stop = System.currentTimeMillis();
System.out.println(stop - start);
System.out.println("insert");
start = System.currentTimeMillis();
for (int i = 0; i < (num = args.length == 1 ? Integer.parseInt(args[0]) :
11); ++i) {
n = rand.nextInt(num);
list.insert(n);
System.out.print(n + ": ");
for (Integer j : list) {
System.out.print(j + " ");
}
System.out.println();
}
stop = System.currentTimeMillis();
System.out.println(stop - start);
System.out.println(list);
}
public class SortedList<E extends Comparable<? super E>> extends List<E>
{
   public Iterator<E> iterator()
   {
      return new Iterator<E>()
      {
         private Node<E> curr = head;
         public boolean hasNext()
         {
            return curr != null;
         }
         public E next()
         {
            E temp = curr.data;
            curr = curr.next; 
            return temp;           
         }
      };
   }
   
   
   public void insert(E data)
   {
      head = insert(head,data);
   }
   private Node<E> insert(Node<E> node, E data)
   {
      if(node == null)
      {
         Node<E> temp = new Node<E>(data); // empty list
         return temp;
      }
      if(data.compareTo(node.data) < 0) // less than value
      {
         Node<E> curr = new Node<E>(data);
         curr.next = node;
         return curr;
      }
      node.next = insert(node.next,data);
      return node;
   }
   
   
   public void remove(E data)
   {
      remove(head,data);
   }
   
   private Node<E> remove(Node<E> curr, E data)
   {
      if(curr == null) //if it is the end of the list, base case
      {
         return null;
      }
      if(data.compareTo(curr.data) == 0) // if node data is = to data we want to remove
      {
         return curr.next; // remove data by skipping over the value
      }
      curr.next = remove(curr.next,data); //continues search with the use of recursion 
      return curr;
   }
   
   
   public E retrieve(int index)
   {
       return retrieve(head ,index, 0);
   }
   private E retrieve(Node<E> curr, int index, int i)
   {
     if(curr == null)
     {
        return null;
     }
     
     if (i == index) // dont neeed if(i <= index && curr.next != null) because we can just increment i in recursive call
     {
         return curr.data;
     }
     return retrieve(curr.next ,index, i + 1); // dont use i ++ because will increment i, i + 1 keeps i same value after call 
   }
   
   
   public boolean search(E data)
   {
      return search(head, data);
   }
   private boolean search(Node<E> curr, E data)
   {
      if(curr != null)
      {
         if(data.compareTo(curr.data) == 0)
         {
            return true;
         }
         return search(curr.next,data);
      }
      return false;
   }
}

}