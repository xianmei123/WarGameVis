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
	vector<Vertex> vertices;
	vector<int> indices;
	int index = 0;
	queue<DataChunk> dataQueue;
	mutex queueMutex;
	const int QUEUE_MAX_SIZE = 5;
	condition_variable full;
	condition_variable empty;
public:
	void readDataFromJson()
	{
		while (true) {
			this->vertices.clear();

			this->indices.clear();
			/*std::string prefix = "../../../data/unit_info_";*/


			std::string fileName = "../../../data1/unit_info_" + std::to_string(this->index) + ".json";
			//std::string fileName = "../../../data1/unit_info_1.json";
			Json::Reader reader;/*���ڰ���JSON���ݸ�ʽ���н���*/
			Json::Value root;/*���ڱ���JSON���͵�һ������*/
			ifstream srcFile(fileName, ios::binary);/*����һ��ifstream���������ļ�demo.json���й���*/
			//char* buffer; //Ҳ���Խ�buffer��Ϊ�������
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
			/*��demo.json���ݽ��������ڵ�root*/

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
					this->vertices.push_back(vertex);
					
			
				}

				time2 = static_cast<float>(glfwGetTime());
				//cout << "get points time : " << time2 - time1 << endl;


				time1 = static_cast<float>(glfwGetTime());
				
				for (int i = 0; i < root["relation"].size(); i++) {
					int id1 = root["relation"][i]["entity1"].asInt();
					int id2 = root["relation"][i]["entity2"].asInt();
					this->indices.push_back(id1);
					this->indices.push_back(id2);
					
				}
				time2 = static_cast<float>(glfwGetTime());
				//cout << "get lines time : " << time2 - time1 << endl;

			}
			srcFile.close();
			DataChunk chunk;
			chunk.vertices = this->vertices;
			chunk.indices = this->indices;
			
			unique_lock<std::mutex> lock(queueMutex);
			full.wait(lock, [this] {
				if (dataQueue.size() >= QUEUE_MAX_SIZE) {
					return false;
				}
				return true;
				});
			dataQueue.push(chunk);
			empty.notify_one();
			//queueMutex.unlock();
			this->index = (index + 1) % 50;
		}
	}
		



	DataChunk getDataChunk() {
		DataChunk chunk;
		unique_lock<std::mutex> lock(queueMutex);
		empty.wait(lock, [this] {
			if (dataQueue.empty()) {
				return false;
			}
			return true;
			});
		chunk = dataQueue.front();
		dataQueue.pop();
		full.notify_one();

		return chunk;

	}
	
	void getDataAsyc() {
		std::thread dataThread(&Data::readDataFromJson, this);
		dataThread.detach();
	}

};





#endif