#include <iostream>
#include <vector>
#include <cstring>
#include <queue>

constexpr static int MaxBoardSize = 200;
constexpr static int ExpectedSolutionSize = 40;

enum FieldStatus {
  Visited, Empty, Blocked, Start, Finish, Enqueued
};

struct BoardField {
  FieldStatus status;
  std::pair<int, int> parent;
};

struct BoardDims {
  int width, height;
};

struct Config {
  BoardField board[MaxBoardSize][MaxBoardSize];
  BoardDims dims;
  std::vector<char> solution = std::vector<char>();
  std::pair<int, int> startPoint;
  std::pair<int, int> finishPoint;
  int robotCount;
  int minDistance;
  int timeLimit;
};

void bfs(Config &config) {
  std::queue<std::pair<int, int>> queue;
  queue.push(std::move(config.startPoint));

//  int i = 1;
  while (!queue.empty()) {
//    std::cout << "Iteration: " << i++ << ' ';
    std::pair<int, int> currentPosition = std::move(queue.front());
//    std::cout << "Considered position: " << currentPosition.first << " " << currentPosition.second << '\n';
    queue.pop();

    if (currentPosition.first == config.finishPoint.first && currentPosition.second == config.finishPoint.second) {
      std::pair<int, int> parent = std::move(config.board[currentPosition.first][currentPosition.second].parent);
      while (parent.first != -1) {
//        std::cout << parent.first << " " << parent.second << '\n';
        if (parent.first > currentPosition.first) {
          config.solution.push_back('G');
        } else if (parent.first < currentPosition.first) {
          config.solution.push_back('D');
        } else if (parent.second > currentPosition.second) {
          config.solution.push_back('L');
        } else {
          config.solution.push_back('P');
        }
        currentPosition = std::move(parent);
        parent = std::move(config.board[currentPosition.first][currentPosition.second].parent);
      }
      return;
    }

    config.board[currentPosition.first][currentPosition.second].status = FieldStatus::Visited;

    if (currentPosition.first + 1 < config.dims.height &&
        config.board[currentPosition.first + 1][currentPosition.second].status == FieldStatus::Empty) {
//      std::cout << "\tAdding " << currentPosition.first + 1 << " " << currentPosition.second << '\n';
      queue.emplace(currentPosition.first + 1, currentPosition.second);
      config.board[currentPosition.first + 1][currentPosition.second].parent = currentPosition;
      config.board[currentPosition.first + 1][currentPosition.second].status = FieldStatus::Enqueued;

    }
    if (currentPosition.first - 1 >= 0 &&
        config.board[currentPosition.first - 1][currentPosition.second].status == FieldStatus::Empty) {
//      std::cout << "\tAdding " << currentPosition.first - 1 << " " << currentPosition.second << '\n';
      queue.emplace(currentPosition.first - 1, currentPosition.second);
      config.board[currentPosition.first - 1][currentPosition.second].parent = currentPosition;
      config.board[currentPosition.first - 1][currentPosition.second].status = FieldStatus::Enqueued;
    }
    if (currentPosition.second + 1 < config.dims.width &&
        config.board[currentPosition.first][currentPosition.second + 1].status == FieldStatus::Empty) {
//      std::cout << "\tAdding " << currentPosition.first << " " << currentPosition.second + 1 << '\n';
      queue.emplace(currentPosition.first, currentPosition.second + 1);
      config.board[currentPosition.first][currentPosition.second + 1].parent = currentPosition;
      config.board[currentPosition.first][currentPosition.second + 1].status = FieldStatus::Enqueued;
    }
    if (currentPosition.second - 1 >= 0 &&
        config.board[currentPosition.first][currentPosition.second - 1].status == FieldStatus::Empty) {
      queue.emplace(currentPosition.first, currentPosition.second - 1);
//      std::cout << "\tAdding " << currentPosition.first << " " << currentPosition.second - 1 << '\n';
      config.board[currentPosition.first][currentPosition.second - 1].parent = currentPosition;
      config.board[currentPosition.first][currentPosition.second - 1].status = FieldStatus::Enqueued;
    }
  }
}

void loadInputFromStdIn(Config &config);

int main(int argc, char * argv[]) {
  Config config;

  loadInputFromStdIn(config);
  bfs(config);

//  std::cout << "BFS ENDED\n";

//  config.solution.shrink_to_fit();
  for (std::vector<char>::reverse_iterator i = config.solution.rbegin(); i != config.solution.rend(); ++i) {
    std::cout << *i;
  }
//  std::cout << '\n';
  return 0;
}

void loadInputFromStdIn(Config &config) {
  // params
  std::cin >> config.dims.height >> config.dims.width;
  std::cin >> config.robotCount;
  std::cin >> config.minDistance;
  std::cin >> config.timeLimit;

  std::string lineBuffer;

  // board
  for (int row = 0; row < config.dims.height; ++row) {
//    std::cin.clear();
//    std::cin.getline(lineBuffer, config.dims.width);
//    printBuffer(lineBuffer, config.dims.width);
    std::cin >> lineBuffer;
//    std::cout << lineBuffer << '\n';
    for (int col = 0; col < config.dims.width; ++col) {
      if (lineBuffer[col] == '.') {
        config.board[row][col].status = FieldStatus::Empty;
//        std::cout << row << ' ' << col << " empty\n";
      } else if (lineBuffer[col] == '#') {
        config.board[row][col].status = FieldStatus::Blocked;
//        std::cout << row << ' ' << col << " blocked\n";
      } else if (lineBuffer[col] >= 'a' && lineBuffer[col] <= 'z') {
        config.board[row][col].status = FieldStatus::Start;
        config.board[row][col].parent = {-1, -1};
        config.startPoint = {row, col};
//        std::cout << row << ' ' << col << " start\n";
      } else if (lineBuffer[col] >= 'A' && lineBuffer[col] <= 'Z') {
//        config.board[row][col].status = FieldStatus::Finish;
        config.board[row][col].status = FieldStatus::Empty;
        config.finishPoint = {row, col};
//        std::cout << row << ' ' << col << " finish\n";
      }
    }
  }
}
