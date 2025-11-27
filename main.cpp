#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <cmath>
#include <map>
#include <fstream>
#include <random>
#include "Graph.h"

using namespace std;

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "SFML Window");


    // Create random verices
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dist(100.0f, 900.0f);
    vector<sf::CircleShape> circles;

    //gives each vertex an ID
    unordered_map<unsigned int, sf::CircleShape> vertexMapper;
    int vertices = 1000;

    for (unsigned int i = 0; i < vertices; i++) {
        float randomX = dist(gen);
        float randomY = dist(gen);
        sf::Vector2f coords = { randomX, randomY };
        sf::CircleShape circle(3.f);
        circle.setFillColor(sf::Color::Red);
        circle.setPosition(coords);
        circles.push_back(circle);
        vertexMapper[i] = circle;
    }
    circles[0].setRadius(5.f);
    circles[1].setRadius(5.f);
    circles[0].setFillColor(sf::Color::Blue);
    circles[1].setFillColor(sf::Color::White);

    vector<Edge> edges;
    vector<sf::VertexArray> lines;
    map<pair<int, int>, int> lineMapper;
    for (unsigned int i = 0; i < circles.size(); i++) {
        for (unsigned int j = i + 1; j < circles.size(); j++) {
            //drawing lines
            if (abs(circles[j].getPosition().x - circles[i].getPosition().x) <= 40 && abs(circles[j].getPosition().y - circles[i].getPosition().y) <= 40) {
                sf::VertexArray line(sf::PrimitiveType::LineStrip, 2);
                line[0].color = sf::Color::Red;
                line[1].color = sf::Color::Red;
                line[0].position = sf::Vector2f(circles[i].getPosition()); // Starting point
                line[1].position = sf::Vector2f(circles[j].getPosition()); // Ending point
                lines.push_back(line);
                //creating edges
                float x_squared = pow(abs(circles[j].getPosition().x - circles[i].getPosition().x), 2);
                float y_squared = pow(abs(circles[j].getPosition().y - circles[i].getPosition().y), 2);
                float weight = pow(x_squared + y_squared, 0.5);
                Edge edge1(i, j, weight);
                Edge edge2(j, i, weight);
                edges.push_back(edge1);
                edges.push_back(edge2);
                lineMapper[{i, j}] = lines.size() - 1;
                lineMapper[{j, i}] = lines.size() - 1;

            }
        }
    }
    //print edges
    for (auto edge : edges) {
        cout << edge.src << " " << edge.dest << " " << edge.weight << endl;
    }


    Graph graph(edges, vertices);
    cout << "Two-way: " << endl << graph.two_way_dijkstra(0, 1, lines, lineMapper) << endl;
    cout << "One-way: " << endl << graph.dijkstra(0, 1, lines, lineMapper) << endl;
    // Main loop
    while (window.isOpen()) {

        // Handle events - don't declare event outside the loop
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        // Clear, draw, display
        window.clear(sf::Color::Black); // Clear with black background

        for (unsigned int i = 0; i < circles.size(); i++) {
            window.draw(circles[i]);
        }
        for (unsigned int i = 0; i < lines.size(); i++) {
            window.draw(lines[i]);
        }

        window.display(); // Show everything
    }

    return 0;
}