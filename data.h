#ifndef DATA_H
#define DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>

#include <direct.h>

#include <vector>
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



class Data {
private:
	vector<Vertex> vertices;
	vector<Line> lines;
	int index = 0;
public:
	void readDataFromJson()
	{
		
		this->vertices.clear();
		this->lines.clear();
		/*std::string prefix = "../../../data/unit_info_";*/
		

		std::string fileName = "../../../data1/unit_info_" + std::to_string(this->index) + ".json";
	
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
			//cout << "Get " + fileName << endl;

		}
		//vector<float> tmpVertices;
		/*��demo.json���ݽ��������ڵ�root*/

		float time1 = static_cast<float>(glfwGetTime());
		float time2 = 0.0f;
		if (reader.parse(srcFile, root))
		{
			
			time2 = static_cast<float>(glfwGetTime());
			cout << "parse file time : " << time2 - time1 << endl;

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
			/*	cout << "id : " << id << endl;
				cout << "posX : " << posX << endl;
				cout << "type : " << type << endl;
				cout << "pos : " << pos.x <<  " " << pos.y << endl;
				cout << "color : " << color.x << endl;*/
			}

			time2 = static_cast<float>(glfwGetTime());
			cout << "get points time : " << time2 - time1 << endl;


			time1 = static_cast<float>(glfwGetTime());
			//cout << root["entity"].size() << endl;
			for (int i = 0; i < root["relation"].size(); i++) {
				int id1 = root["relation"][i]["entity1"].asInt();
				int id2 = root["relation"][i]["entity2"].asInt();
				Line line = Line(this->vertices[id1], this->vertices[id2]);
				this->lines.push_back(line);
				/*cout << "id1 : " << line.vertex1.position.x << " " << line.vertex1.position.y << " " << line.vertex1.position.z << endl;
				cout << "id2 : " << id2 << endl;*/
			}
			time2 = static_cast<float>(glfwGetTime());
			cout << "get lines time : " << time2 - time1 << endl;

		}
		srcFile.close();
		this->index = (index + 1) % 50;
	}

	vector<Vertex> getVertices() {
		return this->vertices;
	}

	vector<Line> getLines() {
		return this->lines;
	}

};





#endif