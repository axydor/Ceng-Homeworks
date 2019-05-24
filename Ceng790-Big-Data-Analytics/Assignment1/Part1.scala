package edu.metu.ceng790.Assignment1

import org.apache.spark.sql.types.IntegerType
import org.apache.spark.sql.types.DateType
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.types.StructType
import org.apache.spark.sql.types.ByteType
import org.apache.spark.sql.types.StructField
import org.apache.spark.sql.types.FloatType
import org.apache.spark.sql.types.StringType
import org.apache.spark.sql.types.LongType
import java.net.URLDecoder
import org.apache.spark.sql.Row
import org.apache.spark.sql.Encoders


object Part1 {
  def main(args: Array[String]): Unit = {

    var spark: SparkSession = null
    try {
      
      spark = SparkSession.builder().appName("Flickr using dataframes").config("spark.master", "local[*]").getOrCreate()

      //   * Photo/video identifier
      //   * User NSID
      //   * User nickname
      //   * Date taken
      //   * Date uploaded
      //   * Capture device
      //   * Title
      //   * Description
      //   * User tags (comma-separated)
      //   * Machine tags (comma-separated)
      //   * Longitude
      //   * Latitude
      //   * Accuracy
      //   * Photo/video page URL
      //   * Photo/video download URL
      //   * License name
      //   * License URL
      //   * Photo/video server identifier
      //   * Photo/video farm identifier
      //   * Photo/video secret
      //   * Photo/video secret original
      //   * Photo/video extension original
      //   * Photos/video marker (0 = photo, 1 = video)

      val customSchemaFlickrMeta = StructType(Array(
        StructField("photo_id", LongType, true),
        StructField("user_id", StringType, true),
        StructField("user_nickname", StringType, true),
        StructField("date_taken", StringType, true),
        StructField("date_uploaded", StringType, true),
        StructField("device", StringType, true),
        StructField("title", StringType, true),
        StructField("description", StringType, true),
        StructField("user_tags", StringType, true),
        StructField("machine_tags", StringType, true),
        StructField("longitude", FloatType, false),
        StructField("latitude", FloatType, false),
        StructField("accuracy", StringType, true),
        StructField("url", StringType, true),
        StructField("download_url", StringType, true),
        StructField("license", StringType, true),
        StructField("license_url", StringType, true),
        StructField("server_id", StringType, true),
        StructField("farm_id", StringType, true),
        StructField("secret", StringType, true),
        StructField("secret_original", StringType, true),
        StructField("extension_original", StringType, true),
        StructField("marker", ByteType, true)))

      val originalFlickrMeta = spark.sqlContext.read
        .format("csv")
        .option("delimiter", "\t")
        .option("header", "false")
        .schema(customSchemaFlickrMeta)
        .load("flickrSample.txt")

      println(originalFlickrMeta.columns.toSeq)
        // YOUR CODE HERE
      val sqlContext = new org.apache.spark.sql.SQLContext(spark.sparkContext)
      import sqlContext.implicits._

      // QUESTION 1
      val first = sqlContext.sql("SELECT photo_id, longitude, latitude, license  FROM flickrMeta")
      first.map(f => "ID: " + f(0) + " - " + "Coord: " + f(1) + " - " + f(2) + " - " +"License: " + f(3)).collect().foreach(println)

      // QUESTION 2
      val pictures = originalFlickrMeta.filter(l => l(15) != null).filter(l => (l.getFloat(10) != 1.0f)).filter(l => l.getFloat(11) != -1.0f)
      pictures.collect()
      pictures.show(12, false)

      // QUESTION 3
      pictures.explain()

      // QUESTION 4  // It works even if we do not perform actions
      println(pictures.count())
      pictures.show()

      // QUESTION 5
      val flickrLicense = spark.sqlContext
          .read
          .format("csv")
          .option("delimeter", " ")
          .option("sep", "\t")
          .option("inferSchema", "true")
          .option("header", "true")
          .load("/home/egemen/IdeaProjects/untitled/src/main/scala/edu/metu/ceng790/Assignment1/FlickrLicense.txt")

      val nonDervLicense = flickrLicense.filter(lis => (lis.getInt(3) == 1)).map(l => (l.getString(0), 1))
      val nonDervs = nonDervLicense.withColumnRenamed("_1", "license")

      //println(flickrLicense.explain())
      val result = pictures.join(nonDervs, Seq("license"))
      //result.show(20)

      result.write.format("csv")
                  .option("delimeter", " ")
                  .option("sep", "\t")
                  .option("header", true)
                  .save("nonDervLicensedPictures.csv")

    } catch {
      case e: Exception => throw e
    } finally {
      spark.stop()
    }
    println("done")
  }
}