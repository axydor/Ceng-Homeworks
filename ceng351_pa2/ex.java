import java.util.ArrayList;



public class ex 
{
    public static void main(String[] args)
    {
        Integer x = 3;
        Person a = new Person("Ali",13);
        Person b = a;
        b.name = "Veli";
        System.out.println(a.name);
        String  y =  String.format("%"+ Integer.toString(x) +"s", Integer.toBinaryString(x)).replace(' ','0');
        System.out.println(y);
    }
}
 
