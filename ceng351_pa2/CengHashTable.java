import java.util.ArrayList;

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
                rehash(this.rows,rowIndex);       
                       
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
        for(int i = 0 ; i < hashPrefixLength; i++ )
        {
            if ( i == splitIndex )
            {
                this.rows.    
            }
        }
    }

	// Own Methods
}
