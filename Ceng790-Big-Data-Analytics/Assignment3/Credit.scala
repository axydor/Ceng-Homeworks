package edu.metu.ceng790.hw3

import org.apache.spark._
import org.apache.spark.ml.Pipeline
import org.apache.spark.ml.classification.{RandomForestClassificationModel, RandomForestClassifier}
import org.apache.spark.ml.evaluation.{BinaryClassificationEvaluator, MulticlassClassificationEvaluator}
import org.apache.spark.ml.feature.{IndexToString, StringIndexer, VectorAssembler}
import org.apache.spark.ml.tuning.{ParamGridBuilder, TrainValidationSplit}
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.SQLContext
import org.apache.spark.sql.functions._
import org.apache.spark.sql.types._
import org.apache.spark.sql._


object Credit {
  // define the Credit Schema
  case class Credit(
    creditability: Double,
    balance: Double, duration: Double, history: Double, purpose: Double, amount: Double,
    savings: Double, employment: Double, instPercent: Double, sexMarried: Double, guarantors: Double,
    residenceDuration: Double, assets: Double, age: Double, concCredit: Double, apartment: Double,
    credits: Double, occupation: Double, dependents: Double, hasPhone: Double, foreign: Double
  )
  // function to create a  Credit class from an Array of Double
  def parseCredit(line: Array[Double]): Credit = {
    Credit(
      line(0),
      line(1) - 1, line(2), line(3), line(4), line(5),
      line(6) - 1, line(7) - 1, line(8), line(9) - 1, line(10) - 1,
      line(11) - 1, line(12) - 1, line(13), line(14) - 1, line(15) - 1,
      line(16) - 1, line(17) - 1, line(18) - 1, line(19) - 1, line(20) - 1
    )
  }
  // function to transform an RDD of Strings into an RDD of Double
  def parseRDD(rdd: RDD[String]): RDD[Array[Double]] = {
    rdd.map(_.split(",")).map(_.map(_.toDouble))
  }

  def main(args: Array[String]) {

        val spark = SparkSession.builder.appName("Spark SQL").config("spark.master", "local[*]").getOrCreate()
				val sc = spark.sparkContext
				val sqlContext = new org.apache.spark.sql.SQLContext(sc)
        spark.sparkContext.setLogLevel("ERROR")

    import sqlContext.implicits._
    // load the data into a  RDD
    val path = "/home/egemen/IdeaProjects/untitled/src/main/scala/edu/metu/ceng790/Assignment3/credit.csv"; // credit.csv
    val creditDF = parseRDD(sc.textFile(path)).map(parseCredit).toDF().cache()

    val features = Array(
      "balance", "duration", "history", "purpose", "amount",
      "savings", "employment", "instPercent", "sexMarried", "guarantors",
      "residenceDuration", "assets", "age", "concCredit", "apartment",
      "credits", "occupation", "dependents", "hasPhone", "foreign")

    // QUESTION 1
    val assembler = new VectorAssembler()
      .setInputCols(features)
      .setOutputCol("features")

    // QUESTION 2
    val indexer = new StringIndexer()
      .setInputCol("creditability")
      .setOutputCol("creditabilityLabel")

    // QUESTION 3
    val Array(trainingData, testData) = creditDF
      .randomSplit(Array(0.8, 0.2))

    // QUESTIONS 4 - 5 - 6
    val rf = new RandomForestClassifier()
      .setLabelCol("creditabilityLabel")
      .setFeaturesCol("features")
      .setSeed(1234)
      .setFeatureSubsetStrategy("auto")

    val pipeline = new Pipeline()
      .setStages(Array(assembler, indexer, rf))

    val binaryEvaluator = new BinaryClassificationEvaluator()
      .setLabelCol("creditabilityLabel")
      .setRawPredictionCol("prediction")

    val paramGrid = new ParamGridBuilder()
      .addGrid(rf.maxBins, Array(25,28,31))
      .addGrid(rf.maxDepth, Array(4,6,8))
      .addGrid(rf.impurity, Array("entropy","gini"))
      .build()

    val trainValidationSplit = new TrainValidationSplit()
      .setEstimator(pipeline)
      .setEvaluator(binaryEvaluator)
      .setEstimatorParamMaps(paramGrid)
      .setTrainRatio(0.75)
      .setParallelism(2)


    // Find the best model
    val model = trainValidationSplit.fit(trainingData)

    // Predict the testData(20% of our data)
    val predictions = model.transform(testData)

    println(model.bestModel.explainParams())

    val accuracy = binaryEvaluator.evaluate(predictions)
    println(s"Accuracy = ${(accuracy)}")

  }


}

