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
            CengHashRow temp = new CengHashRow(String.format("%"+Integer.toString(hashPrefixLength)+"s",Integer.toBinaryString(i)).replace(' ','0'));    
        }
        this.rows.add(temp);
    }

	public void addCoin(CengCoin coin)
	{
		String hashPrefix = Integer.toBinaryString(coin.hashValue()); 			
		String insertTo   = hashPrefix.substring(0,this.hashPrefixLength);// COIN'S HASHPRE
		Integer rowIndex  = Integer.parseInt(insertTo,2)
        CengHashRow which = this.rows.get(rowIndex);
		if (which.getBucket().coins.size() != CengCoinExchange.getBucketSize() )  // NO SPLIT 
		{
            which.getBucket().addCoin(coin);
		}
        else  // TABLE WILL GROW
        {
            if( this.hashPrefixLength == which.getBucket().getHashPrefix())   // CASE 1 ONLY HASHROW POİNTİNG TO THIS BUCKET - TABLE WILL GROW 
            {
                this.hashPrefixLength++;
                rehash(rowIndex);       
                       
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

    public void rehash(int splitIndex)
    {
        for(int i = 0 ; i < pow(2,hashPrefixLength - 1); i++ )
        {
            if ( i == splitIndex )
            {
                CengHashRow newOne = new CengHashRow(this)
                this.rows.get(i);   
            }
            else
            {
                CengHashRow now = this.rows.get(i);
                CengHashRow newOne1 = new CengHashRow(now.getHashPrefix()+"1");
                this.rows.add(newOne1);
                now.setHashPrefix(now.getHashPrefix()+"0")
                for(int j = 0; j < now.getBucket().coinCount(); j++)
                {
                    
                }
            }

        }
    }

	// Own Methods
}
