package edu.metu.ceng790.Assignment1

import org.apache.spark.sql.SparkSession

import org.apache.spark.rdd.RDD

object Part2 {
  def main(args: Array[String]): Unit = {
    var spark: SparkSession = null
    try {
      spark = SparkSession.builder().appName("Flickr using dataframes").config("spark.master", "local[*]").getOrCreate()
      spark.sparkContext.setLogLevel("ERROR")


      val originalFlickrMeta: RDD[String] = spark.sparkContext.textFile("flickrSample.txt")

      // YOUR CODE HERE

      // QUESTION 1
      originalFlickrMeta.take(5).foreach(println)
      println("Count of the RDD: " + originalFlickrMeta.count())

      //QUESTION 2
      val pictures = originalFlickrMeta
                      .map(f => new Picture(f.split("\t")))
                      .filter(f => f.hasValidCountry)
                      .filter(f => f.hasTags)

      //QUESTION 3
      pictures.groupBy(f => (f.c, f.toString)).take(1).foreach(println)

      //QUESTION 4
      val flatted = pictures.map(f => (f.c.toString(), f.userTags))
                            .groupByKey()
                            .map(f => (f._1, f._2.flatten))
                            //.foreach(println)

      //QUESTION 5
      flatted.map(f => (f._1, f._2.groupBy(x=>x).map(y => (y._1, y._2.size) ) ) ).foreach(println)

    } catch {
      case e: Exception => throw e
    } finally {
      spark.stop()
    }
    println("done")
  }
}