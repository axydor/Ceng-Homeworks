public class CengHashRow {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public String hashPrefix()
	{
		// TODO: Return row's hash prefix (such as 0, 01, 010, ...)
		return this.hashPrefix;	
	}
	
	public CengBucket getBucket()
	{
		// TODO: Return the bucket that the row points at.
		return this.bucket;		
	}
	
	public boolean isVisited()
	{
		// TODO: Return whether the row is used while searching.
		return this.isVisited;		
	}

    public void setBucket(CengHashRow row)
    {
        this.bucket = row.getBucket();
        this.isVisited = row.isVisited();
        this.hashPrefix = row.hashPrefix();
    }

    public void setHashPrefix(String hashPrefix)
    {
        this.hashPrefix = hashPrefix;    
    }
	
	// Own Methods

	private String hashPrefix;
	private Boolean isVisited;
	private CengBucket bucket;

	public CengHashRow(String hashPrefix){
		this.hashPrefix = hashPrefix;
		this.bucket = new CengBucket(hashPrefix.length());
	}
}
