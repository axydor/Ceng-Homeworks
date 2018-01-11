import java.util.ArrayList;

public class CengBucket {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int coinCount()
	{
		// TODO: Return the coin count in the bucket.
		return this.bucket.size();
	}
	
	public CengCoin coinAtIndex(int index)
	{
		// TODO: Return the corresponding coin at the index.
		return this.bucket.get(index);
	}
	
	public int getHashPrefix()
	{
		// TODO: Return hash prefix length.
		return this.hashPrefixLength;
	}
	
	public Boolean isVisited()
	{
		// TODO: Return whether the bucket is found while searching.
		return this.isVisited;		
	}
	
	// Own Methods

	public int hashPrefixLength;
	public Boolean isVisited;
	public ArrayList<CengCoin> coins;

	public CengBucket(int hashPrefixLength)
	{
		this.hashPrefixLength = hashPrefixLength;
		this.coins = new ArrayList<CengCoin>();
	}

	public void addCoin(CengCoin coin)
	{
		
		this.coins.add(coin);
	}
}
