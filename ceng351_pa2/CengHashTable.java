import java.util.ArrayList;
import java.lang.Math;

public class CengHashTable {

	public Integer hashPrefixLength;
	public ArrayList<CengHashRow> rows;

	public CengHashTable()
	{
		this.rows = new ArrayList<CengHashRow>();
		this.hashPrefixLength = 0;
		CengHashRow temp = new CengHashRow("");
		this.rows.add(temp);
		// TODO: Create a hash table with only 1 row.
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
	        	for(int k=0; k < which.getBucket().coinCount(); k++)    // Move Coins
	          	{
                	CengCoin currCoin = which.getBucket().coinAtIndex(k);
                    Integer hashOfCoin = currCoin.hashValue();
                    String hashPreOfCoin = String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(hashOfCoin)).replace(' ','0');
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
			String hashPrefix = String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(coin.hashValue())).replace(' ','0'); 	//    COIN'S HASHPRE //4 = 100
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
		// TODO: Empty Implementation
	}
	
	public void print()
	{
		// TODO: Empty Implementation
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
				System.out.println("\t\t\t"+"<hash>"+String.format("%"+Integer.toString(3)+"s",Integer.toBinaryString(coin.hashValue())).replace(' ','0') + "</hash>");
				System.out.println("\t\t\t"+"<value>"+Integer.toString(coin.key())+"|" + coin.name()+"|"+ coin.value() + "|" + coin.currency() +"</value>");
				System.out.println("\t\t"+"</coin>");

			}
			System.out.println("\t\t"+"</coin>");
			System.out.println("\t</bucket>");
			System.out.println("</row>");
		}
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
        for(int i = 0 ; i < Math.pow(2,hashPrefixLength - 1); i++ )   // FOR LOOP UNTIL THE 2^HASH_PREFIX_LENGTH(BEFORE GROWING)
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
                    String hashPreOfCoin = String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(hashOfCoin)).replace(' ','0');
                   
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
