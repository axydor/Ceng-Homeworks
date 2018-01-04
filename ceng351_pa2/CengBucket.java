public class CengBucket {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int coinCount()
	{
		// TODO: Return the coin count in the bucket.
		return bucket.size();
	}
	
	public CengCoin coinAtIndex(int index)
	{
		// TODO: Return the corresponding coin at the index.
		return this.bucket.get(index);
	}
	
	public int getHashPrefix()
	{
		// TODO: Return hash prefix length.
		return hashPrefixLength;
	}
	
	public Boolean isVisited()
	{
		// TODO: Return whether the bucket is found while searching.
		return isVisited;		
	}
	
	// Own Methods
	//public int hashPrefix ; ?
	public int hashPrefixLength;
	public Boolean isVisited;

	public CengBucket()
	{
		ArrayList<CengCoin> bucket = new ArrayList<CengCoin>();
	}
}
