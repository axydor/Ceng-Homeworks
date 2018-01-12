public class ex 
{
    public static void main(String[] args)
    {
        Integer x = 3;
        String  y =  String.format("%"+ Integer.toString(x) +"s", Integer.toBinaryString(x)).replace(' ','0');
        System.out.println(y);
    }
}
 
