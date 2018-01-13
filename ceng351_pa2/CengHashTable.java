import java.util.ArrayList;
import java.lang.Math;

public class CengHashTable {

	public Integer hashPrefixLength;
	public ArrayList<CengHashRow> rows;
    public int hashLen = (int)(Math.log(CengCoinExchange.getHashMod()) / Math.log(2));

	public CengHashTable()
	{
		this.rows = new ArrayList<CengHashRow>();
		this.hashPrefixLength = 0;
		CengHashRow temp = new CengHashRow("");
		this.rows.add(temp);
	}


	public void addCoin(CengCoin coin)
	{
		// If HashLength is 3, hashPrefix of 2 : 010, hashPre: 010 -> rowIndex = 2
		// If HashLength is 2, hashPrefix of 2 : 01,   hashPre: 01  -> rowIndex = 1
		if(hashPrefixLength == 0)
		{
			CengHashRow which = this.rows.get(0);
			if(which.getBucket().coinCount() == CengCoinExchange.getBucketSize() )
			{
				this.hashPrefixLength++;
				CengHashRow newOne = new CengHashRow("1");
                which.setHashPrefix("0");
                which.getBucket().hashPrefixLength++;
	        	for(int k=0; k < which.getBucket().coinCount(); k++)    // Move Coins
	          	{
                	CengCoin currCoin = which.getBucket().coinAtIndex(k);
                    Integer hashOfCoin = currCoin.hashValue();
                    String hashPreOfCoin = String.format("%1s",Integer.toBinaryString(hashOfCoin)).replace(' ','0');
	        		if( hashPreOfCoin == "1")
	        		{
	        			which.getBucket().removeCoin(k);
	          			newOne.getBucket().addCoin(currCoin);
	           		}
	           	}
               	this.rows.add(newOne);			
                addCoin(coin);	
            }
			else
			{
				which.getBucket().addCoin(coin);
			}
		}
		else
		{
			String hashPrefix = String.format("%"+Integer.toString(hashLen)+"s",Integer.toBinaryString(coin.hashValue())).replace(' ','0'); 	//    COIN'S HASHPRE //4 = 100
			String hashPre    = hashPrefix.substring(0,hashPrefixLength);
			Integer rowIndex  = Integer.parseInt(hashPre,2);  								   
	        CengHashRow which = this.rows.get(rowIndex);
			if (which.getBucket().coinCount() != CengCoinExchange.getBucketSize() )  // NO SPLIT 
			{
	            which.getBucket().addCoin(coin);
			}
	        else  // BUCKET IS FULL - SPLIT WILL HAPPEN
	        {
	            if( this.hashPrefixLength == which.getBucket().getHashPrefix())  // CASE 1 - ONLY 1 HASHROW IS POİNTİNG TO THIS BUCKET 
	            {															    // THE TABLE WILL GROW 
	                this.hashPrefixLength++;  
	                rehash(rowIndex);       
	                addCoin(coin);
                    //System.out.println("CASE1");
                    //System.out.println("hashPreLen: " +Integer.toString(hashPrefixLength));
                    //System.out.println("rowwINdex:  "+Integer.toString(rowIndex));
                }   
	            else                                                     // CASE 2 - MORE THAN 1 HASHROW POINTS SAME BUCKET J < I
	            {													    //  TABLE WILL NOT GROW
	            	Integer preLen = which.getBucket().getHashPrefix();   // OLD HASHTABLE_PREFIX_LENGTH
	            	String buckPre = which.hashPrefix().substring(0,preLen);      // 2:01  -> 0
	            	for(int j=0; j < this.rows.size(); j++)
	            	{
	            		if( rows.get(j).hashPrefix().substring(0,preLen) == buckPre )
	            		{
	            			which.getBucket().hashPrefixLength++;
	            			CengBucket newOne = new CengBucket(which.getBucket().getHashPrefix());
	            		         				
	            			for(int i=j; i < Math.pow(2,this.hashPrefixLength - preLen ); i++)
	            			{
	            				if (i <  Math.pow(2,this.hashPrefixLength - preLen ) / 2 )
	            				{
	            				this.rows.get(i).setBucket(which.getBucket());
	            				}
	            				else
	            				{
	            				this.rows.get(i).setBucket(newOne);
	            				}
	            			}
	            			for(int k=0; k < which.getBucket().coinCount(); k++)    // Move Coins
	            			{
	            				CengCoin move = which.getBucket().coinAtIndex(k);
	            				which.getBucket().removeCoin(k);
	            				addCoin(move);
	            			}
	            		}
	            	}

	            	Integer newHashPreLen = which.getBucket().getHashPrefix();
	            	for(int j=0; j < which.getBucket().coinCount(); j++)
	            	{
						
	            	}
	            }
	        }
	    }

	}
	
	public void searchCoin(Integer key)
	{
        Boolean found = false;
	    System.out.println("BEGIN TABLE");
        
        String hashPrefix = String.format("%"+Integer.toString(hashLen)+"s",Integer.toBinaryString(key)).replace(' ','0'); 	//    Key 2 - - -> 010 If hashMod is 8
        Integer rowIndex;  								   
        if(hashPrefixLength == 0)
        {
            rowIndex = 0;   
        }
        else
        {
            String hashPre    = hashPrefix.substring(0,hashPrefixLength ) ;
            rowIndex  = Integer.parseInt(hashPre,2);  								   
        }
        CengHashRow which = this.rows.get(rowIndex);
        CengBucket bucket = which.getBucket();
        
    
        for(int j=0; j < bucket.coinCount(); j++)
        {
            CengCoin coin = bucket.coinAtIndex(j);
            if( coin.key() == key )
            {
                found = true;
                System.out.println("<row>");
                System.out.println("\t<hashPrefix>" + which.hashPrefix() + "</hashPrefix>");
                System.out.println("\t<bucket>");
                System.out.println("\t\t<hashLength>"+ Integer.toString(bucket.getHashPrefix())  + "</hashLength>");
                System.out.println("\t\t<coin>");
                System.out.println("\t\t\t"+"<hash>"+String.format("%"+Integer.toString(hashLen)+"s",Integer.toBinaryString(coin.hashValue())).replace(' ','0') + "</hash>");
                System.out.println("\t\t\t"+"<value>"+Integer.toString(coin.key())+"|" + coin.name()+"|"+ coin.value() + "|" + coin.currency() +"</value>");
                System.out.println("\t\t</coin>");
                System.out.println("\t</bucket>");
                System.out.println("</row>");
            }
        }

        if( found == false )
        {
            System.out.println("None");
        }
	    System.out.println("END TABLE");
    }
	
	public void print()
	{
	    System.out.println("BEGIN TABLE");
    
    	for(int i=0; i < rowCount(); i++)
		{
			System.out.println("<row>");
			CengHashRow row = this.rows.get(i);
			System.out.println("\t"+"<hashPrefix>"+ row.hashPrefix() + "</hashPrefix>");
			System.out.println("\t<bucket>");
			CengBucket bucket = row.getBucket();
			System.out.println("\t\t"+"<hashLength>"+ Integer.toString(row.getBucket().getHashPrefix())+"</hashLength>");
			for(int j=0; j < bucket.coinCount();j++)
			{
				CengCoin coin = bucket.coinAtIndex(j);
				System.out.println("\t\t"+"<coin>");
				System.out.println("\t\t\t"+"<hash>"+String.format("%"+Integer.toString(hashLen)+"s",Integer.toBinaryString(coin.hashValue())).replace(' ','0') + "</hash>");
				System.out.println("\t\t\t"+"<value>"+Integer.toString(coin.key())+"|" + coin.name()+"|"+ coin.value() + "|" + coin.currency() +"</value>");
				System.out.println("\t\t"+"</coin>");

			}
			System.out.println("\t</bucket>");
			System.out.println("</row>");
		}
	    System.out.println("END TABLE");
	}

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int prefixBitCount()
	{
		// TODO: Return table's hash prefix length.
		return hashPrefixLength;		
	}
	
	public int rowCount()
	{
		// TODO: Return the count of HashRows in table.
		return rows.size();		
	}
	
	public CengHashRow rowAtIndex(int index)
	{
		// TODO: Return corresponding hashRow at index.
		return this.rows.get(index);
	}

	// Own Methods

	// SIZE OF THE TABLE GROWS 
    public void rehash(int splitIndex)
    {                                               
        for(int i = 0 ; i < Math.pow(2,hashPrefixLength - 1) + 1; i+=2 )   // FOR LOOP UNTIL THE 2^HASH_PREFIX_LENGTH(BEFORE GROWING)
        {
            if ( i == splitIndex )   // SPLIT WILL OCCUR 
            {
            	CengHashRow now = this.rows.get(i);
            	String oldPrefix = now.hashPrefix();
                CengHashRow newOne = new CengHashRow(oldPrefix+"1");
                now.setHashPrefix(oldPrefix+"0");
                for(int j = 0; j < now.getBucket().coinCount(); j++)   // SPLIT COINS ACCORDING TO THE NEW PREFIX LENGTH
                {
                	CengCoin currCoin = now.getBucket().coinAtIndex(j);
                    Integer hashOfCoin = currCoin.hashValue();
                    String hashPreOf = String.format("%"+Integer.toString(hashLen)+"s",Integer.toBinaryString(hashOfCoin)).replace(' ','0');
                    String hashPreOfCoin = hashPreOf.substring(0,hashPrefixLength);
	                if (hashPreOfCoin == (oldPrefix + "1") )    // MOVE COIN TO THE CORRECT POSITION
	                {
	                	newOne.getBucket().addCoin(currCoin);
	                	now.getBucket().removeCoin(j);  // REMOVING FROM THE PREVIOUS POSITION
	                } 

	            }
	            now.getBucket().hashPrefixLength++;            // LENGTH OF COMMON HASH PREFIX FOR THE BUCKET INCREASES
	            newOne.getBucket().hashPrefixLength = hashPrefixLength; 
                this.rows.add(i+1,newOne);   // ADDING NEW HASHROW AFTER CURRENT ROW

            }
            else         // HANDLING NEW ROW'S POINTERS - NO SPLIT HERE
            {           // NEW HASHROWS WILL POINT TO SAM BUCKET
                CengHashRow now = this.rows.get(i);
                CengHashRow newOne = new CengHashRow(now.hashPrefix()+"1");
                newOne.setBucket(now.getBucket());
                this.rows.add(i+1,newOne);   // ADDING AFTER CURRENT ROW
                now.setHashPrefix(now.hashPrefix()+"0");
            }

        }
    }

}
