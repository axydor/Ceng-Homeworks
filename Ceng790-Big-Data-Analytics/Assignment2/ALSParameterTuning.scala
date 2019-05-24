package edu.metu.ceng790.Assignment2

import java.io.FileWriter

import breeze.linalg.shuffle
import org.apache.spark.sql.{Row, SparkSession}
import org.apache.spark.mllib.recommendation.{ALS, Rating}
import org.apache.spark.rdd.RDD

object ALSParameterTuning {

  // Function to take square of a Double
  def sqr(x:Double):Double = return x*x

  // Create Rating object from Row
  def parseRating(fields: Row): Rating = {
    Rating(fields(0).toString.toInt, fields(1).toString.toInt, fields(2).toString.toDouble)
  }

  // Read ratings from csv file and create RDD[Rating]
  def readUserRatings(location:String, spark: SparkSession): RDD[Rating] = {
    val ratings = spark.read.option("header", "true").csv(location).rdd.map(parseRating)
    return ratings
  }

  // Normalize ratings by dividing each user's rating by user's average rating
  def normalizeRatings(ratings : RDD[Rating]) : RDD[Rating] = {
      // We group by each user in order to take average of each user's ratings.
      val ratings_per_user = ratings.groupBy(f => f.user).map( x => (x._1, x._2.map( r => r.rating).sum / x._2.size ) )

      // Collect as Map in order to access with index like and Array, RDD.lookup does produce errors
      val x = ratings_per_user.collect().toMap

      // Normalize user's ratings
      val normalizedRatings = ratings.map( f => Rating(f.user, f.product, f.rating / x(f.user) ) )

      return normalizedRatings
  }

  def main(args: Array[String]): Unit = {

    var spark: SparkSession = null
    var fw :FileWriter = null

    try {
      spark = SparkSession.builder().appName("Movielens data").config("spark.master", "local[*]").getOrCreate()

      spark.sparkContext.setLogLevel("ERROR")

      /*
        Important Note
        Usages of the functions related to ALS are inspired from spark's document while studying
        https://spark.apache.org/docs/latest/ml-collaborative-filtering.html
      */

      val start = System.nanoTime()
      val ratings = readUserRatings("/home/egemen/IdeaProjects/untitled/src/rating.csv", spark)

      val normalized_ratings = normalizeRatings(ratings)

      // Categorize 90% of our data as training set and other 10% as test set.
      val Array(train, test) = normalized_ratings.randomSplit(Array(0.9, 0.1))
      train.cache()
      test.cache()

      // Train our model with 12 times with different parameters
      // Save the results into a file so that we can choose the best option

      val ranks = Array(8, 12)
      val lambdas = Array(0.01, 1, 10)
      val iterations = Array(10, 20)

      fw = new FileWriter("MSE_Results.txt", true)

      println("RANK ---- LAMBDA --- ITERATION ---- MSE" )
      fw.write("RANK ---- LAMBDA --- ITERATION ---- MSE\n" )

      for(i <- ranks) {
        for(j <- lambdas) {
          for(k <- iterations) {
            // Statistics about the runtime of training
            val start = System.nanoTime()

            // Train our model with 90% of our data
            val als_model = ALS.train(train, i, k, j)
            //als_model.save(spark.sparkContext, "als_model" )

            // Shape our data by removing rating So that we wil predict the ratings for them
            val usersProducts = test.map(f => (f.user, f.product))

            // Predict
            val predictions = als_model.predict(usersProducts).map(f => ((f.user, f.product), f.rating))

            // We hold (user, movie) as a Key and (real rating, predicted rating) pair as Tuple
            val real_and_predictions = test.map(f => ((f.user, f.product),f.rating)).join(predictions)

            // Calculate Mean Square Error
            val mean_square_err = real_and_predictions.map(f => sqr(f._2._1 - f._2._2)).mean()

            print(i.toString + " -------- " + j.toString + " --------" + k.toString +  " -------- ")
            println(mean_square_err.toString + "\n")
            println("Time elapsed: " + (System.nanoTime()-start)/1e9 )

            fw.write(i.toString + " -------- " + j.toString + " --------" + k.toString +  " -------- ")
            fw.write(mean_square_err.toString + "\n")
            fw.write("Time elapsed: " + ((System.nanoTime()-start)/1e9).toString + "\n" )
          }
        }
      }

    } catch {
      case e: Exception => throw e
    } finally {
      val end = System.nanoTime()
      spark.stop()
      fw.close()
    }
    println("done")
  }
}
