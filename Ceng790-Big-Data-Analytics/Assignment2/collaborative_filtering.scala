package edu.metu.ceng790.Assignment2

import java.io.FileWriter

import breeze.linalg.shuffle
import org.apache.spark.mllib.recommendation.{ALS, MatrixFactorizationModel, Rating}
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.{Row, SparkSession}

import scala.util.control.Breaks._
import scala.collection.mutable.ArrayBuffer

case class Movie(movieId:Int, title:String, genre:Array[String])

object collaborative_filtering {

  def parseMovie(fields: Row): Movie = {
    Movie(fields(0).toString.toInt, fields(1).toString, fields(2).toString.split("\\|"))
  }


  def readMovies(location:String, spark: SparkSession): RDD[Movie] = {
    val movies = spark.read.option("header", "true").csv(location).rdd.map(parseMovie)

    return movies
  }

  def elicitateRatings(selectedMovies: Map[Int, String], spark: SparkSession): RDD[Rating] = {
    var userId = 0
    var userRatingForSelectedMovies  = ArrayBuffer.empty[Rating]
    var i = 1
    for(movie <- selectedMovies) {
      breakable {
       while(true) {
         try {
           println(i.toString + ") What is your rating for the movie " + movie._2 + "\"" )
           val rating = scala.io.StdIn.readDouble()
           if(rating < 5.1) {
             userRatingForSelectedMovies += Rating(0, movie._1, rating)
             i += 1
             break
           }
         } catch {
           case e: Exception => println("Please Give correct input");
         }
       }
      }
    }
    return spark.sparkContext.parallelize(userRatingForSelectedMovies)
  }

  def chooseTopRatedMovies(ratings : RDD[Rating], movies : RDD[Movie], i: Int): Map[Int, String] = {
    // Create mostRatedMovies as an Array(movieId, Number Of ratings)
    val mostRatedMovies = ratings.groupBy(_.product).map(f=> (f._1, f._2.size)).takeOrdered(200)(Ordering[Int].reverse.on(_._2))

    // Select 40 of them
    val selectedMovies = shuffle(mostRatedMovies).map(f => (f._1, movies.filter(_.movieId == f._1)
                                               .map(m => m.title )
                                               .take(1)(0) ) ).take(i).toMap
    return selectedMovies
  }

  def main(args: Array[String]): Unit = {

    var spark: SparkSession = null
    var fw :FileWriter = null
    val start = System.nanoTime()

    try {
        spark = SparkSession.builder().appName("Movielens data").config("spark.master", "local[*]").getOrCreate()

        spark.sparkContext.setLogLevel("ERROR")

        val movies = collaborative_filtering.readMovies("/home/egemen/IdeaProjects/untitled/src/movie.csv", spark)
        movies.cache()

        val ratings = ALSParameterTuning.readUserRatings("/home/egemen/IdeaProjects/untitled/src/rating.csv", spark)

        val selectedMovies = chooseTopRatedMovies(ratings, movies, 40)

        // Ask user to rate 40 top rated movie
        val userRatingForSelectedMovies = elicitateRatings(selectedMovies, spark)

        // Add user to the database
        val newRatings = ratings.union(userRatingForSelectedMovies)

        val normalizedRatings = ALSParameterTuning.normalizeRatings(newRatings)

        // Train the model
        val model = ALS.train(normalizedRatings, 12, 10 , 0.01)

        // Recommend 20 movies to the user
        val recommendations = model.recommendProducts(0, 20)

        // Print recommended movies to the user
        recommendations.map(f => movies.filter(m => m.movieId == f.product).take(1)(0).title).foreach(println)

    } catch {
      case e: Exception => throw e
    } finally {
      val end = System.nanoTime()
      println("Time elapsed: " + (end-start)/1e9 )
      spark.stop()
    }
    println("done")
  }
}
