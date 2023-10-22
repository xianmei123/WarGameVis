#ifndef DATA_PRODUCER_H
#define DATA_PRODUCER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>

#include <direct.h>

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <map>

using namespace std;

struct Vertex {
	// position
	glm::vec3 position;
	// normal
	glm::vec3 color;

	Vertex(const glm::vec3& position, const glm::vec3& color)
		: position(position), color(color) {}
};

struct Line {

	Vertex vertex1;

	Vertex vertex2;

	Line(const Vertex& vertex1, const Vertex& vertex2)
		: vertex1(vertex1), vertex2(vertex2) {}
};

struct DataChunk {
	vector<Vertex> vertices;
	vector<int> indices;
};


class Data {
private:
	
	int index = 0;
	int readIndex = 0;
	queue<DataChunk> dataQueue;
	mutex queueMutex;
	mutex indexMutex;
	const int QUEUE_MAX_SIZE = 5;
	condition_variable full;
	condition_variable empty;
	map<int, DataChunk> dataMap;
public:
	void readDataFromJson()
	{
		while (true) {
			vector<Vertex> vertices;
			vector<int> indices;
			/*std::string prefix = "../../../data/unit_info_";*/
			
			unique_lock<std::mutex> indexlock(indexMutex);
			int localIndex = this->index;
			this->index = (index + 1) % 50;
			
			std::string fileName = "../../../data1/unit_info_" + std::to_string(localIndex) + ".json";
			indexlock.unlock();
			//std::string fileName = "../../../data1/unit_info_1.json";
			Json::Reader reader;/*用于按照JSON数据格式进行解析*/
			Json::Value root;/*用于保存JSON类型的一段数据*/
			ifstream srcFile(fileName, ios::binary);/*定义一个ifstream流对象，与文件demo.json进行关联*/
			//char* buffer; //也可以将buffer作为输出参数
			//if ((buffer = getcwd(NULL, 0)) == NULL)
			//{
			//	perror("getcwd error");
			//}
			//else
			//{
			//	printf("path:%s\n", buffer);
			//	free(buffer);
			//}

			if (!srcFile.is_open())
			{

				cout << "Fail to open " + fileName << endl;
				return;
			}
			else
			{
				cout << "Get " + fileName << endl;

			}
			//vector<float> tmpVertices;
			/*将demo.json数据解析到根节点root*/

			float time1 = static_cast<float>(glfwGetTime());
			float time2 = 0.0f;
			if (reader.parse(srcFile, root))
			{

				time2 = static_cast<float>(glfwGetTime());
				//cout << "parse file time : " << time2 - time1 << endl;

				time1 = static_cast<float>(glfwGetTime());
				float scale = 2000.0;
				for (int i = 0; i < root["entity"].size(); i++) {
					int id = root["entity"][i]["id"].asInt();
					float posX = (root["entity"][i]["posX"].asDouble() - 5000.0) / scale;
					float posY = (root["entity"][i]["posY"].asDouble() - 5000.0) / scale;
					float posZ = root["entity"][i]["posZ"].asDouble();
					string type = root["entity"][i]["type"].asString();
					glm::vec3 pos = glm::vec3(posX, posY, posZ);
					glm::vec3 color = type == "Unit" ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
					Vertex vertex = Vertex(pos, color);
					vertices.push_back(vertex);
					
			
				}

				time2 = static_cast<float>(glfwGetTime());
				//cout << "get points time : " << time2 - time1 << endl;


				time1 = static_cast<float>(glfwGetTime());
				
				for (int i = 0; i < root["relation"].size(); i++) {
					int id1 = root["relation"][i]["entity1"].asInt();
					int id2 = root["relation"][i]["entity2"].asInt();
					indices.push_back(id1);
					indices.push_back(id2);
					
				}
				time2 = static_cast<float>(glfwGetTime());
				//cout << "get lines time : " << time2 - time1 << endl;

			}
			srcFile.close();
			DataChunk chunk;
			chunk.vertices = vertices;
			chunk.indices = indices;
			
			unique_lock<std::mutex> lock(queueMutex);
			/*full.wait(lock, [this] {
				if (dataMap.size() >= QUEUE_MAX_SIZE) {
					return false;
				}
				return true;
				});*/
			full.wait(lock, [this, localIndex] {
				if (localIndex - readIndex > 10 || (readIndex > localIndex && readIndex - localIndex < 40)) {
					return false;
				}
				return true;
				});
			dataMap.insert(std::pair<int, DataChunk>(localIndex, chunk));
			cout << "insert chunk indices : " << dataMap[localIndex].indices.size() << "chunk vertices : " << chunk.vertices.size() << " localIndex:"<<  localIndex << endl;
			cout << fileName << " size: " << dataMap.size() << endl;
			empty.notify_one();
			//queueMutex.unlock();
			
		}
	}
		



	DataChunk getDataChunk() {
		DataChunk chunk;
		unique_lock<std::mutex> lock(queueMutex);
		empty.wait(lock, [this] {
			if (dataMap.count(readIndex) == 0) {
				return false;
			}
			return true;
			});

		cout << "readIndex : " << readIndex << "size: " << dataMap.size() << endl;
		chunk = dataMap[readIndex];
		cout << "get chunk indices : " << chunk.indices.size() << "chunk vertices : " << chunk.vertices.size() << endl;
		dataMap.erase(readIndex);
		readIndex = (readIndex + 1) % 50;
		full.notify_one();

		return chunk;

	}
	
	void getDataAsyc() {
		std::thread dataThread(&Data::readDataFromJson, this);
		dataThread.detach();
	}

};





#endif