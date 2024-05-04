// test1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>


// Структура для представления точки с координатами x и y
struct Point {
    double x;
    double y;
};

// Структура для представления линии, которая содержит начальную и конечную точки
struct Line {
    Point start;
    Point end;
    std::string name;
};

struct NamePoint {
    Point coord;
    std::string name;
};

struct Cell {
    Point bottomLeft;
    Point topRight;
    std::string name;
    std::vector<Line> elementsLines;
    std::vector<NamePoint> contactPoints; 
    std::string specification; 
};

struct Data {
    std::vector<Line> borderLines;   
    std::vector<Line> elementsLines;
    std::vector<Cell> cells; 
    std::vector<NamePoint> cellPoints; 
    std::vector<NamePoint> contactPoints;
};


// Функция для обработки строки и извлечения координат и имени
void parseBorderLine(std::string& line, Data& data, size_t& posB1)  {
    double x1, y1, x2, y2;
    std::string name; 
    line = line.substr(posB1);
    if (sscanf_s(line.c_str(), "%[^(](%lf,%lf) X(%lf);", name.c_str(), sizeof(name.c_str()), &x1, &y1, &x2) == 4) {   
        data.borderLines.push_back({ {x1, y1}, {x2, y1}, name.c_str()});
        
    }
    else if (sscanf_s(line.c_str(), "%[^(](%lf,%lf) Y(%lf);", name.c_str(), sizeof(name.c_str()), &x1, &y1, &y2) == 4) {
        data.borderLines.push_back({ {x1, y1}, {x1, y2}, name.c_str()});
        
    }
}
void parseWireLine(std::string& line, Data& data, size_t& pos) {
    double x1, y1, x2, y2;
    std::string nameWire,wwire,type;
    line = line.substr(pos);
    if (sscanf_s(line.c_str(), "Wire(%[^)]); W_WIRE(%[^)]) %[^(](%lf,%lf) X(%lf);", nameWire.c_str(), sizeof(nameWire.c_str()), wwire.c_str(), sizeof(wwire.c_str()), type.c_str(), sizeof(type.c_str()), &x1, &y1, &x2) == 6) {
        std::string name = "Wire(" + std::string(nameWire.c_str()) + "); W_WIRE(" + std::string(wwire.c_str()) + ") " + std::string(type.c_str());   
        data.elementsLines.push_back({ {x1, y1}, {x2, y1}, name });

    }
    else if (sscanf_s(line.c_str(), "Wire(%[^)]); W_WIRE(%[^)]) %[^(](%lf,%lf) Y(%lf);", nameWire.c_str(), sizeof(nameWire.c_str()), wwire.c_str(), sizeof(wwire.c_str()), type.c_str(), sizeof(type.c_str()), &x1, &y1, &y2) == 6) { 
        std::string name = "Wire(" + std::string(nameWire.c_str()) + "); W_WIRE(" + std::string(wwire.c_str()) + ") " + std::string(type.c_str()); 
        data.elementsLines.push_back({ {x1, y1}, {x1, y2}, name }); 

    }
}
void parseWWireLine(std::string& line, Data& data, size_t& pos) {
    double x1, y1, x2, y2;
    std::string  wwire, type; 
    line = line.substr(pos);
    if (sscanf_s(line.c_str(), "W_WIRE(%[^)]) %[^(](%lf,%lf) X(%lf);", wwire.c_str(), sizeof(wwire.c_str()), type.c_str(), sizeof(type.c_str()), &x1, &y1, &x2) == 5) { 
        std::string name =  "W_WIRE(" + std::string(wwire.c_str()) + ") " + std::string(type.c_str()); 
        data.elementsLines.push_back({ {x1, y1}, {x2, y1}, name });

    }
    else if (sscanf_s(line.c_str(), "W_WIRE(%[^)]) %[^(](%lf,%lf) Y(%lf);", wwire.c_str(), sizeof(wwire.c_str()), type.c_str(), sizeof(type.c_str()), &x1, &y1, &y2) == 5) { 
        std::string name = "W_WIRE(" + std::string(wwire.c_str()) + ") " + std::string(type.c_str()); 
        data.elementsLines.push_back({ {x1, y1}, {x1, y2}, name }); 

    }
}

void parsePointName(std::string& line, Data& data, size_t& pos) {  
    double x1, y1;
    std::string  name; 
    line = line.substr(pos);
    if (sscanf_s(line.c_str(), "TB(%lf,%lf, \"%[^\"]\");", &x1, &y1, name.c_str(), sizeof(name.c_str())) == 3) {
        data.cellPoints.push_back({ {x1, y1}, name.c_str()});
    }
    else if(sscanf_s(line.c_str(), "OR(%*[^)]) %[^(](%lf,%lf);", name.c_str(), sizeof(name.c_str()), &x1, &y1) == 3) { 
        data.contactPoints.push_back({ {x1, y1}, name.c_str()});
    }
}

Data readLinesFromFile(const std::string& filename) { 
    Data data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find("B1(");  
        if (pos != std::string::npos)  
        {
            parseBorderLine(line, data, pos);
            continue;
        }
        pos = line.find("Wire(");
        if (pos != std::string::npos)
        {
            parseWireLine(line, data, pos);
            continue;
        }
        pos = line.find("W_WIRE(");  
        if (pos != std::string::npos) 
        {
            parseWWireLine(line, data, pos); 
            continue;
        }
        pos = line.find("TB("); 
        if (pos != std::string::npos) 
        {
            parsePointName(line, data, pos); 
            continue;
        }
        pos = line.find("OR(");
        if (pos != std::string::npos)
        {
            parsePointName(line, data, pos);
            continue;
        }
    }
        
    file.close();
    return data;
}

 
// Функция для нахождения точек пересечения линий
std::vector<Point> findIntersectionPoints(const std::vector<Line>& lines) {
    std::vector<Point> intersectionPoints;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].name=="B1")
        {
            for (size_t j = i + 1; j < lines.size(); ++j) {
                if (lines[j].name == "B1")
                {
                    const Line& line1 = lines[i];
                    const Line& line2 = lines[j];
                    // Проверка на пересечение линий
                    double det = (line1.end.x - line1.start.x) * (line2.end.y - line2.start.y) -
                        (line2.end.x - line2.start.x) * (line1.end.y - line1.start.y);
                    if (det == 0) continue; // Линии параллельны
                    double t = ((line2.start.x - line1.start.x) * (line2.end.y - line2.start.y) -
                        (line2.start.y - line1.start.y) * (line2.end.x - line2.start.x)) / det;
                    double u = -((line1.start.x - line1.end.x) * (line1.start.y - line2.start.y) -
                        (line1.start.y - line1.end.y) * (line1.start.x - line2.start.x)) / det;
                    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
                        double x = line1.start.x + t * (line1.end.x - line1.start.x);
                        double y = line1.start.y + t * (line1.end.y - line1.start.y);
                        intersectionPoints.push_back({ x, y });
                    }
                }
            }
        }
        
    }

    std::sort(intersectionPoints.begin(), intersectionPoints.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
        });

    return intersectionPoints;
}

//bool pointInsideCell(const Point& point, const Cell& cell) {
//    // Проверяем, находится ли точка внутри прямоугольника, образованного ячейкой
//    return point.x >= cell.bottomLeft.x && point.x <= cell.topRight.x &&
//        point.y >= cell.bottomLeft.y && point.y <= cell.topRight.y;
//} 

bool pointInsideCell(const Point& point, const Point& bottomLeft, const Point& topRight) {
    // Проверяем, находится ли точка внутри прямоугольника, образованного ячейкой
    return point.x >= bottomLeft.x && point.x <= topRight.x &&
        point.y >= bottomLeft.y && point.y <= topRight.y;
}

//std::vector<Line> findElementsInsideCell(const std::vector<Cell>& cells, const std::vector<Line>& elementsLines) {
//    std::vector<Line> linesInsideCells;
//
//    for (const auto& cell : cells) {
//        for (const auto& line : elementsLines) {
//            // Проверяем, находятся ли начальная и конечная точки линии внутри текущей ячейки
//            if (pointInsideCell(line.start, cell) || pointInsideCell(line.end, cell)) { 
//                linesInsideCells.push_back(line);
//            }
//        }
//    }
//
//    return linesInsideCells;
//}


Point getLimitPoint(const Point& bottomLeft, const Point& topRight, Point& elementPoint) {
    if (bottomLeft.x > elementPoint.x) elementPoint.x = bottomLeft.x;
    if (bottomLeft.y > elementPoint.y) elementPoint.y = bottomLeft.y;
    if (topRight.x < elementPoint.x) elementPoint.x = topRight.x; 
    if (topRight.y < elementPoint.y) elementPoint.y = topRight.y; 
    return elementPoint;
}

Line getLimitElementsLine(const Point& bottomLeft, const Point& topRight, const Line& elementsLine) {
     Line line = elementsLine;
    line.start = getLimitPoint(bottomLeft, topRight, line.start);
    line.end = getLimitPoint(bottomLeft, topRight, line.end);
    return line;
}

std::vector<Line> findElementsInsideCell(const Point& bottomLeft, const Point& topRight, const std::vector<Line>& elementsLines) {  
    std::vector<Line> linesInsideCells; 

    for (const auto& line : elementsLines) {
        // Проверяем, находятся ли начальная и конечная точки линии внутри текущей ячейки
        if (pointInsideCell(line.start, bottomLeft, topRight) || pointInsideCell(line.end, bottomLeft, topRight)) {
            linesInsideCells.push_back(getLimitElementsLine(bottomLeft, topRight, line));
        }
    }

    return linesInsideCells; 
}

std::vector<NamePoint> findContactsInsideCell(const Point& bottomLeft, const Point& topRight, const std::vector<NamePoint>& contacts) {
    std::vector<NamePoint> contInsideCell;
    for (const auto& contPoint : contacts) {
        // Проверяем, находится ли точка внутри текущей ячейки
        if (pointInsideCell(contPoint.coord, bottomLeft, topRight)) {
            contInsideCell.push_back(contPoint); 
        }
    }
    return contInsideCell;
}

std::string findCellName(const Point& bottomLeft, const Point& topRight, Data data) {
    for (const auto& namePoint : data.cellPoints) {
        if (bottomLeft.x < namePoint.coord.x && bottomLeft.y < namePoint.coord.y && topRight.y > namePoint.coord.y) return namePoint.name;
    }
    return "";
}


std::string getCellsSpecification(const Point& bottomLeft, const Point& topRight, std::vector<Line>& elementsLines) {
    std::string specification;
    for (const auto& el : elementsLines) {
        std::ostringstream x, y;
        y << std::fixed << std::setprecision(2) << el.start.y;
        x << std::fixed << std::setprecision(2) << el.start.x;
        if (bottomLeft.x == el.start.x || bottomLeft.x == el.end.x)
        {
            specification += el.name + "--lft--" + y.str() + "\n";
        }
        if (bottomLeft.y == el.start.y || bottomLeft.y == el.end.y)
        {
            specification += el.name + "--btm--" + x.str() + "\n";
        }
        if (topRight.y == el.start.y || topRight.y == el.end.y)
        {
            specification += el.name + "--top--" + x.str() + "\n";
        }
        if (topRight.x == el.start.x || topRight.x == el.end.x)
        {
            specification += el.name + "--rht--" + y.str() + "\n";
        }
    }
    return specification;
} 
std::string getNewCellsSpecification(const Cell& cell) { 
    std::string newSpecification;
    std::vector<Line> el = cell.elementsLines;
    newSpecification = getCellsSpecification(cell.bottomLeft, cell.topRight, el); 
    return newSpecification;
}


std::vector<Cell> findCellCoordinates(const std::vector<Point>& intersectionPoints, Data data) {
    
    
    for (size_t i = 0; i < intersectionPoints.size(); ++i) {
        
        for (size_t j = i + 1; j < intersectionPoints.size(); ++j) {
            // Проверяем, является ли точка верхним правым углом для текущей точки
            if (intersectionPoints[j].x <= intersectionPoints[i].x ||
                intersectionPoints[j].y <= intersectionPoints[i].y) {
                continue;
            }
            std::string name = findCellName(intersectionPoints[i], intersectionPoints[j], data);
            std::vector<Line> elements = findElementsInsideCell(intersectionPoints[i], intersectionPoints[j], data.elementsLines);
            std::vector<NamePoint> contacts = findContactsInsideCell(intersectionPoints[i], intersectionPoints[j], data.contactPoints);
            std::string specification = getCellsSpecification(intersectionPoints[i], intersectionPoints[j], elements);
            data.cells.push_back({ intersectionPoints[i], intersectionPoints[j], name, elements, contacts, specification });    
            break;
        }
    }
    std::sort(data.cells.begin(), data.cells.end(), [](const Cell & cell1, const Cell & cell2) {
        return  cell1.bottomLeft.y > cell2.bottomLeft.y; // Сравниваем имена ячеек
    });
    return data.cells; 
}


void moveCellToOrigin(Cell& cell) {
    // Найдем сдвиг для перемещения нижнего левого угла ячейки в (0,0)
    double offsetX = cell.bottomLeft.x;
    double offsetY = cell.bottomLeft.y;

    // Применим сдвиг ко всем точкам границ ячейки
    cell.bottomLeft.x -= offsetX;
    cell.bottomLeft.y -= offsetY;
    cell.topRight.x -= offsetX;
    cell.topRight.y -= offsetY;

    // Применим сдвиг ко всем точкам внутри ячейки
    for (auto& line : cell.elementsLines) {
        line.start.x -= offsetX;
        line.start.y -= offsetY;
        line.end.x -= offsetX;
        line.end.y -= offsetY;
    }
    for (auto& contact : cell.contactPoints) {
        contact.coord.x -= offsetX;
        contact.coord.y -= offsetY;
    }
    cell.specification = getNewCellsSpecification(cell);
}

std::vector<Cell> moveAllCellsToOrigin(std::vector<Cell>& cells) { 
    std::vector<Cell> newCells = cells;
    // Применим сдвиг ко всем элементам внутри каждой ячейки
    for (auto& cell : newCells) {
        moveCellToOrigin(cell);
    }
    return newCells;
}

int main() {
    
    Data data = readLinesFromFile("coordinates.txt");
    setlocale(LC_ALL, "");
    if (data.borderLines.empty()) { 
        std::cerr << "Ошибка чтения координат из файла" << std::endl;
        return 1;
    }
    std::cout << "Координаты линий:" << std::endl;
    for (const auto& b : data.borderLines) {
        std::cout << b.name << ": (" << b.start.x << ", " << b.start.y << ") - ("
            << b.end.x << ", " << b.end.y << ")" << std::endl;
    }
    for (const auto& el : data.elementsLines) {
        std::cout << el.name << ": (" << el.start.x << ", " << el.start.y << ") - ("
            << el.end.x << ", " << el.end.y << ")" << std::endl;
    }

    std::cout << "Контакты одноточечные:" << std::endl;
    for (size_t i = 0; i < data.contactPoints.size(); ++i) {
        const NamePoint& point = data.contactPoints[i];
        std::cout << point.name << ": (" << point.coord.x << ", " << point.coord.y << ")"  << std::endl;
    }

    std::vector<Point> intersectionPoints = findIntersectionPoints(data.borderLines);
    if (intersectionPoints.empty()) {
        std::cerr << "Линии не пересекаются, невозможно определить ячейки" << std::endl;
        return 1;
    }
    std::cout << "Координаты пересечения:" << std::endl;
    int i = 0;
    for (const auto& intPoints : intersectionPoints) {
        std::cout << "(" << intPoints.x << ", " << intPoints.y << ")" << std::endl; 
        
    }

    ////std::vector<Point> cellCoordinates = findCellCoordinates(intersectionPoints);
    std::vector<Cell> cells = findCellCoordinates(intersectionPoints,data);   
    data.cells = cells;
    // Вывод координат ячеек
    std::cout << "Координаты ячеек:" << std::endl; 
    for (const auto& cell : cells) {
        std::cout << "Ячейка " << cell.name << ": (" << cell.bottomLeft.x << ", " << cell.bottomLeft.y << ") - ("
            << cell.topRight.x << ", " << cell.topRight.y << "):"  << std::endl; 
        std::cout << "Элементы ячейки:" << std::endl;  
        for (const auto& el : cell.elementsLines) {
            std::cout << el.name << ": (" << el.start.x << ", " << el.start.y << ") - ("
                << el.end.x << ", " << el.end.y << ")" << std::endl;
        } 
        if (cell.contactPoints.size())
        {
            std::cout << "Одноточечные контакты ячейки:" << std::endl;
            for (const auto& cont : cell.contactPoints) {
                std::cout << cont.name << ": (" << cont.coord.x << ", " << cont.coord.y << ")"  << std::endl;
            }
        }
        std::cout << std::endl;
    }

    std::vector<Cell> newCells = moveAllCellsToOrigin(data.cells); 

    // Вывод координат ячеек
    std::cout << "Координаты ячеек, смещенных в начало координат:" << std::endl;
    for (const auto& cell : newCells) {
        std::cout << "Ячейка " << cell.name << ": (" << cell.bottomLeft.x << ", " << cell.bottomLeft.y << ") - ("
            << cell.topRight.x << ", " << cell.topRight.y << "):" << std::endl;
        std::cout << "Элементы ячейки:" << std::endl;
        for (const auto& el : cell.elementsLines) {
            std::cout << el.name << ": (" << el.start.x << ", " << el.start.y << ") - ("
                << el.end.x << ", " << el.end.y << ")" << std::endl;
        }
        if (cell.contactPoints.size())
        {
            std::cout << "Одноточечные контакты ячейки:" << std::endl;
            for (const auto& cont : cell.contactPoints) {
                std::cout << cont.name << ": (" << cont.coord.x << ", " << cont.coord.y << ")" << std::endl;
            }
        }
        std::cout << "Спецификация ячеки:\n" << cell.specification;


        std::cout << std::endl;

    } 

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
