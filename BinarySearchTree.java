import java.util.*;

public class BinarySearchTree<E extends Comparable<? super E>> extends BinaryTree<E> 
{
   private Node<E> root = null; // start root as null so Main can acess root, will be overided in insert method
   
   public Iterator<E> iterator()
   {
      vector = new Vector<E>();
      traverse(root);
      return vector.iterator();
   }
   
   private void traverse(Node<E> curr) 
   {
      if (curr != null) 
      {
         traverse(curr.left);
         vector.add(curr.data);
         traverse(curr.right);
      }
   }
   private Vector<E> vector;

      
   public void insert(E data)
   {
      root = insert(root,data); // assigning root nod of tree and recursively insertnig it 
   }
   private Node <E> insert(Node<E> node, E data) 
   {
      if(node == null)
      {
         return new Node<E>(data); // create a node and return the value
      }
      
      if(data.compareTo(node.data) <= 0) // this compares the new data with the parent/ root   // add duplicates to right subtree
      {
         node.left = insert(node.left, data); // insert into left subtree
      }
      
      if(data.compareTo(node.data) > 0) // insert right subtree
      {
         node.right = insert(node.right, data);
      }
      return node; //
   }
   
   
   public boolean search(E data)
   {
      return search(root,data);
   }
   private boolean search(Node<E> node, E data)
   {
      if(node != null)
      {
         if(data.compareTo(node.data) < 0)//lessthan current node data
         {
            return search(node.left, data);
         }
         else if(data.compareTo(node.data) > 0)//greaterthan current node data
         {
           return search(node.right,data);
         }
         else // found node data
         {
            return true;
         }
      }
      else
      {
         return false;
      }
   }
   
   
   public void remove(E data)
   {
      remove(root,data);
   }
   private Node<E> remove(Node<E> node, E data)
   {
      if(node != null)
      {
         if(data.compareTo(node.data) == -1)
         {
            node.left = remove(node.left,data);
         }
         if(data.compareTo(node.data) == 1)
         {
            node.right = remove(node.right, data);
         }
         
         if(data.compareTo(node.data) == 0)
         {
            if(node.left == null && node.right == null) // if the parent has 0 or 1 child
            {
               return null;
            }
            else if(node.left == null)
            {
               return node.right;
            }
            else if(node.right == null)
            {
               return node.left;
            }
            else
            {
               Node<E> pred = findIOP(node.left); //predessor node
               node.data = pred.data;
               node.left = remove(node.left,pred.data);

               
               /* not fully recursive
               while(pred.right != null)
               {
                  pred = pred.right; 
               }
               node.data = pred.data;
              */
            }
            
           }
        }
      return node;
    }
    
    private Node<E> findIOP(Node<E> node) 
    {
      if (node.right == null) 
      {
        return node;  // right most value in left subtree
      }
    return findIOP(node.right);
   }  
}
