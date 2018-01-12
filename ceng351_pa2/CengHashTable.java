import java.util.ArrayList;

public class CengHashTable {

	public Integer hashPrefixLength;
	public ArrayList<CengHashRow> rows;

	public CengHashTable()
	{
		this.rows = new ArrayList<CengHashRow>();
		this.hashPrefixLength = 0;
		CengHashRow temp = new CengHashRow(0);
		this.rows.add(temp);
		// TODO: Create a hash table with only 1 row.
	}

    public CengHashTable(Integer hashPrefixLength)
    {
		this.rows = new ArrayList<CengHashRow>();
		this.hashPrefixLength = hashPrefixLength;
		for (int i  = 0; i < pow(2,hashPrefixLength); i++ )
        {
        	//  Create new HashRow with prefix 
            CengHashRow temp = new CengHashRow(String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(i)).replace(' ','0'));    
	        this.rows.add(temp);
        }
    }

	public void addCoin(CengCoin coin)
	{
		String hashPrefix = Integer.toBinaryString(coin.hashValue()); 	//    COIN'S HASHPRE //4 = 100
		String insertTo   = hashPrefix.substring(0,this.hashPrefixLength);                  // 4:1_00 
		Integer rowIndex  = Integer.parseInt(insertTo,2)  								   // rows[1]
        CengHashRow which = this.rows.get(rowIndex);
		if (which.getBucket().coins.size() != CengCoinExchange.getBucketSize() )  // NO SPLIT 
		{
            which.getBucket().addCoin(coin);
		}
        else  // BUCKET IS FULL - SPLIT WILL HAPPEN
        {
            if( this.hashPrefixLength == which.getBucket().getHashPrefix())   // CASE 1 ONLY HASHROW POİNTİNG TO THIS BUCKET 
            {																 // THE TABLE WILL GROW 
                this.hashPrefixLength++;  
                rehash(rowIndex);       
            }   
            else                                                          // CASE 2 J < I
            {															 //  TABLE WILL NOT GROW
            	CengBucket newOne = new CengBucket(which.getBucket().getHashPrefix());
            	for(j=rowIndex; j < rowIndex+(this.hashPrefixLength - which.getBucket().getHashPrefix()))
            	{
            		
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

	// SIZE OF THE TABLE GROWS 
    public void rehash(int splitIndex)
    {                                               
        for(int i = 0 ; i < pow(2,hashPrefixLength - 1); i++ )   // FOR LOOP UNTIL THE 2^HASH_PREFIX_LENGTH(BEFORE GROWING)
        {
            if ( i == splitIndex )   // SPLIT WILL OCCUR 
            {
            	String oldPrefix = now.getHashPrefix();
            	CengHashRow now = this.rows.get(i);
                CengHashRow newOne1 = new CengHashRow(oldPrefix+"1");
                now.setHashPrefix(oldPrefix+"0");
                for(int j = 0; j < now.getBucket().coinCount(); j++)   // SPLIT COINS ACCORDING TO THE NEW PREFIX LENGTH
                {
                	CengCoin currCoin = now.getBucket().coinAtIndex(j)
                    Integer hashOfCoin = currCoin.hashValue();
                    String hashPreOfCoin = String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(hashOfCoin)).replace(' ','0');
                   
	                if (hashPreOfCoin == (oldPrefix + "1") )    // MOVE COIN TO THE CORRECT POSITION
	                {
	                	newOne1.getBucket().addCoin(currCoin);
	                	now.getBucket().removeCoin(j);  // REMOVING FROM THE PREVIOUS POSITION
	                } 

	            }
	            now.getBucket().hashPrefixLength++;            // LENGTH OF COMMON HASH PREFIX FOR THE BUCKET INCREASES
	            now1.getBucket().hashPrefixLength = hashPrefixLength; 
                this.rows.add(i+1,newOne1);   // ADDING NEW HASHROW AFTER CURRENT ROW

            }
            else         // HANDLING NEW ROW'S POINTERS - NO SPLIT HERE
            { 
                CengHashRow now = this.rows.get(i);
                CengHashRow newOne1 = new CengHashRow(now.getHashPrefix()+"1");
                this.rows.add(i+1,newOne1);   // ADDING AFTER CURRENT ROW
                now.setHashPrefix(now.getHashPrefix()+"0");
                newOne1.setBucket(now);
            }

        }
    }

	// Own Methods
}
