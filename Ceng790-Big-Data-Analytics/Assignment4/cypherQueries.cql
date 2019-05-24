// Visualising the graph
MATCH grap=(c1:Character)-[weight:INTERACTS]->(c2:Character) RETURN grap

// Summary statistics for the minimum, maximum and average number of characters each character has interacted with
MATCH (c1:Character)-[weight:INTERACTS]->(c2:Character) RETURN c1, count(weight) ORDER BY(count(weight)) DESC
MATCH (c1:Character)-[weight:INTERACTS]->(c2:Character) RETURN c1, avg(weight.weight)

// Shortest Path
MATCH (source:Character {name: "Arya"}), (dest:Character {name:"Ramsay"})
CALL algo.shortestPath.stream(source, dest, "weight")
YIELD nodeId, cost
RETURN algo.getNodeById(nodeId).name AS  name, cost

// All Pair Shortest Path
// Longest Shortest Path between two characters
CALL algo.allShortestPaths.stream('weight', {nodeQuery:'Character', defaultValue:null})
YIELD sourceNodeId, targetNodeId, distance
WITH sourceNodeId, targetNodeId, distance
WHERE algo.isFinite(distance) = true

MATCH (source:Character) WHERE id(source) = sourceNodeId
MATCH (target:Character) WHERE id(target) = targetNodeId
WITH source, target, distance WHERE source <> target

RETURN source.name AS source, target.name AS target, distance
ORDER BY distance DESC
LIMIT 1

// Characters with interaction distance 4 to Cersei Lannister
MATCH (source:Character {name:"Cersei"})
CALL algo.shortestPaths.stream(source, "weight", {nodeQuery:'Character'})
YIELD nodeId, distance
WITH source, nodeId, distance

MATCH (target:Character) WHERE id(target) = nodeId
WITH source, target, distance WHERE source <> target AND distance = 4

RETURN source.name AS source, target.name AS target, distance
ORDER BY distance DESC

// Jon Snow's parents
MATCH (john:Character )-[relation:RELATIONSHIP]->(relative:Character {name:"Jon"}) RETURN john.name, relation, relative.name

// Create Siblings
MATCH (ch1:Character)-[relation:RELATIONSHIP]->(ch2:Character)
MATCH (ch3:Character)-[rel:RELATIONSHIP]->(ch4:Character)
WHERE ch1.name = ch3.name AND ch2.name <> ch4.name
CREATE (ch2)-[sib:SIBLINGS {parent: ch1.name}]->(ch4)
RETURN ch2.name, ch4.name, relation.tie, ch1.name

// Find Incestious Relationships
MATCH (father)-[s:SIBLINGS]->(mother), (mother)-[r1:RELATIONSHIP {tie:"mother"}]->(child), (father)-[r2:RELATIONSHIP]->(child)
RETURN DISTINCT father.name, mother.name

