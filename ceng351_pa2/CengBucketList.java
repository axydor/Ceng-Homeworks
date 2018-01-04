public class CengBucketList {

	public void addCoin(CengCoin coin)
	{
		// TODO: Empty Implementation
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
	
	public int bucketCount()
	{
		// TODO: Return all bucket count.
		return bucketList.size();
	}
	
	public CengBucket bucketAtIndex(int index)
	{
		// TODO: Return corresponding bucket at index.
		return bucketList.get(index);
	}
	
	// Own Methods

	public CengBucketList()
	{
		ArrayList<CengBucket> bucketList = new ArrayList<CengBucket>();
	}
}
