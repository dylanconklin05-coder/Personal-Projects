import java.util.*;

//in private methods use node.next, dont use head 

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
