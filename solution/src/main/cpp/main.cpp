#include <iostream>
#include <vector>
#include <array>
#include <queue>

constexpr static int MaxBoardSize = 12;
constexpr static int MaxRobotCount = 3;

enum FieldStatus {
  Visited, Empty, Blocked, Start, Finish, Enqueued
};

double inline squaredDistance(const std::pair<int, int> &positionOne, const std::pair<int, int> &positionTwo) {
  return (positionOne.first - positionTwo.first) * (positionOne.first - positionTwo.first) +
      (positionOne.second - positionTwo.second) * (positionOne.second - positionTwo.second);
}

struct BoardField {
  std::array<FieldStatus, MaxRobotCount> statuses;
//  std::array<std::pair<int, int>, MaxRobotCount> parents;
  // rozważyć od razu uzupełnianie literkami (nie ma potrzeby potem tego odtwarzać)
  std::array<char, MaxRobotCount> parents;
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
  double minDistance;
  int timeLimit;
};

struct Case {
  BoardField board[MaxBoardSize][MaxBoardSize];
  BoardDims dims;

};

void bfs(Config &config) {
  std::queue<std::pair<int, int>> queue;
  queue.push(std::move(config.startPoint));

  while (!queue.empty()) {
    std::pair<int, int> currentPosition = std::move(queue.front());
    queue.pop();

    if (currentPosition.first == config.finishPoint.first && currentPosition.second == config.finishPoint.second) {
      std::pair<int, int> parent = std::move(config.board[currentPosition.first][currentPosition.second].parent);
      while (parent.first != -1) {
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
      queue.emplace(currentPosition.first + 1, currentPosition.second);
      config.board[currentPosition.first + 1][currentPosition.second].parent = currentPosition;
      config.board[currentPosition.first + 1][currentPosition.second].status = FieldStatus::Enqueued;

    }
    if (currentPosition.first - 1 >= 0 &&
        config.board[currentPosition.first - 1][currentPosition.second].status == FieldStatus::Empty) {
      queue.emplace(currentPosition.first - 1, currentPosition.second);
      config.board[currentPosition.first - 1][currentPosition.second].parent = currentPosition;
      config.board[currentPosition.first - 1][currentPosition.second].status = FieldStatus::Enqueued;
    }
    if (currentPosition.second + 1 < config.dims.width &&
        config.board[currentPosition.first][currentPosition.second + 1].status == FieldStatus::Empty) {
      queue.emplace(currentPosition.first, currentPosition.second + 1);
      config.board[currentPosition.first][currentPosition.second + 1].parent = currentPosition;
      config.board[currentPosition.first][currentPosition.second + 1].status = FieldStatus::Enqueued;
    }
    if (currentPosition.second - 1 >= 0 &&
        config.board[currentPosition.first][currentPosition.second - 1].status == FieldStatus::Empty) {
      queue.emplace(currentPosition.first, currentPosition.second - 1);
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

  for (auto i = config.solution.rbegin(); i != config.solution.rend(); ++i) {
    std::cout << *i;
  }
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
    std::cin >> lineBuffer;
    for (int col = 0; col < config.dims.width; ++col) {
      if (lineBuffer[col] == '.') {
        config.board[row][col].status = FieldStatus::Empty;
      } else if (lineBuffer[col] == '#') {
        config.board[row][col].status = FieldStatus::Blocked;
      } else if (lineBuffer[col] >= 'a' && lineBuffer[col] <= 'z') {
        config.board[row][col].status = FieldStatus::Start;
        config.board[row][col].parent = {-1, -1};
        config.startPoint = {row, col};
      } else if (lineBuffer[col] >= 'A' && lineBuffer[col] <= 'Z') {
        config.board[row][col].status = FieldStatus::Empty;
        config.finishPoint = {row, col};
      }
    }
  }
}
