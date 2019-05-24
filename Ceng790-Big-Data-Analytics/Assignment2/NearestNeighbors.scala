package edu.metu.ceng790.Assignment2

import java.util.NoSuchElementException

import org.apache.spark.mllib.recommendation.Rating
import org.apache.spark.sql.SparkSession
import org.apache.spark.rdd.RDD

import util.control.Breaks._
import scala.collection.mutable.ArrayBuffer

object NearestNeighbors {


  /*
   *  Calculating the Cosine similarity between two User vectors
   *  User Vectors contains features as genre and magnitude i.e count of that genre
   *
   *  Cos(0) = A.B / (|A|.|B|)
   *  A.B = \sum Ai * Bi
   *  |A| = \sqrt ( \sum Ai^2 )
   *  |B| = \sqrt ( \sum Bi^2 )
   */
  def userSim(userVector1: Map[String,Int], userVector2: Map[String,Int]) : Double = {
    var xy : Double = 0 // \sum Ai * Bi
    var A2 : Double = 0 // \sum Ai**2
    var B2 : Double = 0 // \sum Bi**2
    for (elem <- userVector1) {
      breakable {
        val x = elem._2
        var y = 0
        try {
          y = userVector2(elem._1)
        } catch {
          case key : NoSuchElementException =>  break // continue
        }
        xy += x * y
        A2 += ALSParameterTuning.sqr(x)
      }
    }
    for (elem <- userVector2) {
      B2 += ALSParameterTuning.sqr(elem._2)
    }
    val sqrtA = math.sqrt(A2)
    val sqrtB = math.sqrt(B2)

    if ( sqrtA == 0 || sqrtB == 0)
      return 0.0

    return xy / (sqrtA * sqrtB)
  }

  /*
   * Recommend movies to a user according to similar Users(k many) that we have chosen before by Cosine sim.
   * Use weighted average to find rating for each movie.
   * Sort movies according to the calculated ratings and return 20 of them
   */
  def recommendMovies(similarUsers:ArrayBuffer[(Int, Double)],
                      movieNames:Map[Int, String],
                      goodRatings:Map[Int, Iterable[Rating]] ) : ArrayBuffer[(String, Double)] = {

    var movieRatings = ArrayBuffer.empty[(String, Double)]
    for ( movie <- movieNames ) {
      var weightedRating = 0.0
      var totalSimilarity = 0.0
      for ( user <- similarUsers.toMap )  {
        var rating = goodRatings(user._1).filter(_.product == movie._1)
        rating.foreach( rat => { weightedRating +=  user._2 * rat.rating; totalSimilarity += user._2 } )
     }
      //println(weightedRating, totalSimilarity)
      if(totalSimilarity != 0) {
        val possibleRating = weightedRating / totalSimilarity
        movieRatings += ((movie._2, possibleRating))
      }
    }
    val recommendations = movieRatings.sortBy(x => x)(Ordering[Double].reverse.on(_._2)).take(20)
    return recommendations
  }

  /*
   * Finding the most similar k-many users to testUser by cosine similarity
   */
  def knn(testUser : Map[String, Int], k:Int, usersVectors : RDD[(Int, Map[String, Int])] ) : ArrayBuffer[(Int, Double)] = {
    var userIdSimilarities = ArrayBuffer.empty[(Int, Double)]
    for (user <- usersVectors.collectAsMap()) {
      val sim = userSim(testUser, user._2)
      userIdSimilarities += ((user._1, sim))
    }
    val similarUsers = userIdSimilarities.sortBy(x => x)(Ordering[Double].reverse.on(_._2)).take(k)

    return similarUsers
  }

  def rddLoop(ratings: RDD[Rating]) = {
    var i = 0
    println(i)
  }

  def main(args: Array[String]): Unit = {

    var spark: SparkSession = null
    val start = System.nanoTime()

    try {
      spark = SparkSession.builder().appName("NearestNeighbors").config("spark.master", "local[*]").getOrCreate()

      spark.sparkContext.setLogLevel("ERROR")

      val movies = collaborative_filtering.readMovies("/home/egemen/IdeaProjects/untitled/src/movie.csv", spark)

      var ratings = ALSParameterTuning.readUserRatings("/home/egemen/IdeaProjects/untitled/src/rating.csv", spark)
      rddLoop(ratings);
      /*
      // Select top 40 rated movies
      val selectedMovies = collaborative_filtering.chooseTopRatedMovies(ratings, movies, 40)
      // Ask User to choose movies
      val userRatingForSelectedMovies = collaborative_filtering.elicitateRatings(selectedMovies, spark)

      ratings = ratings.union(userRatingForSelectedMovies)

      // avgRatings holds (userId, average of user's ratings, user's ratings)
      val avgRatings = ratings.groupBy(r => r.user).map(x => (x._1, x._2.map(c => c.rating/x._2.size).reduce((a,b) => (a+b)), x._2))

      // Select ratings for each user that those ratings have higher rating value than user's average rating
      // This holds whole ratings that are above average
      val avgRatings2 = avgRatings.flatMap(x=> (x._3.filter(y => y.rating >= x._2)))

      // We groupBy for user again
      val goodRatings = avgRatings2.groupBy(_.user)

      val movieNames = movies.map(m => (m.movieId, m.title))
      val movieGenres = movies.map(m => (m.movieId, m.genre))

      // movieId, rating
      val ratedMovieGenres = avgRatings2.map(rat => (rat.product, rat))

      // We flatten ratings for each user
      // userId, Genres of a movie as an array of string
      val userGenres = ratedMovieGenres.join(movieGenres).map(x => (x._2._1.user, x._2._2)).groupByKey()

      // userId, list of genres flatttened as (genre,1) so there may be multiple tuples with same genre
      // For example for userId = 1  ----->  RDD(1, [("comedy",1), ("drama",1), ("comedy",1)]  )
      val userGenresWithCount1 = userGenres.map(u => (u._1, u._2.flatMap(genreArr => genreArr.map(genre => (genre,1) )  ) ))

      // UserId, list of Genres with counts of each genre
      val usersVector = userGenresWithCount1.map(u => (u._1, u._2.groupBy(_._1).map(x=> (x._1, x._2.size) )))

      val user = usersVector.collectAsMap()(0)

      println("Our User's taste i.e genre vector")
      println(user)

      val similarUsers = knn(user, 10, usersVector)

      val movieNamesMap = movieNames.collectAsMap() -- selectedMovies.keySet

      val recommendations = recommendMovies(similarUsers, movieNamesMap.toMap, goodRatings.collect().toMap)

      recommendations.foreach(println)
*/
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
