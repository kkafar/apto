#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>
#include <cmath>

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

  void SetStatusForAll(FieldStatus &&status) {
    // rozważyć ręczne rozwinięcie tej pętli
    // for (auto &s : statuses) s = status;
    statuses[0] = statuses[1] = statuses[2] = status;
  }

  void inline SetStatusForRobot(char robotCharId, FieldStatus &&status) {
    statuses[robotCharId - 'a'] = status; 
  }

  void inline SetStatusForRobot(int robotId, FieldStatus &&status) {
    statuses[robotId] = status;
  }

  void inline SetStatusesInorder(FieldStatus &&first, FieldStatus &&second, FieldStatus &&third) {
    statuses[0] = first;
    statuses[1] = second;
    statuses[2] = third;
  }

  void inline SetParentsForAll(char parent) {
    parents[0] = parents[1] = parents[2] = parent;
  }
};

struct BoardDims {
  int width, height;
};

struct Config {
  std::vector<std::vector<BoardField>> board;
  BoardDims dims;
  std::vector<std::vector<char>> solution = std::vector<std::vector<char>>();
  std::vector<char> solution = std::vector<char>();
  std::vector<std::pair<int, int>> startPositions;
  std::vector<std::pair<int, int>> finishPositions;
  // std::array<std::pair<int, int>, MaxRobotCount> startPositions;
  // std::array<std::pair<int, int>, MaxRobotCount> finishPosition;
  int robotCount;
  double minDistance;
  int timeLimit;

  Config(BoardDims dims, int robotCount, double minDistance, int timeLimit) : 
      dims(dims),
      robotCount(robotCount),
      minDistance(minDistance),
      timeLimit(timeLimit) {
    board.resize(dims.height);
    for (auto &vec : board) vec.resize(dims.width);

    startPositions.resize(robotCount);
    finishPositions.resize(robotCount);
  }
};

struct Case {
  std::vector<std::vector<BoardField>> board;
  std::vector<std::pair<int, int>> currentPositions;
  BoardDims dims;
  int time;
};

struct MoveCombinationProvider {
  std::vector<std::pair<int, int>> moves;
  std::vector<int> indices;
  std::size_t currentPoss;
  std::size_t possCount;

  MoveCombinationProvider(int moveCount) {
    moves = std::vector<std::pair<int, int>>{
      {0, 1},
      {0, -1},
      {1, 0},
      {-1, 0},
      {0, 0}
    };
    indices = std::vector<int>(moveCount);
    std::fill(begin(indices), end(indices), 0);
    currentPoss = 0;
    possCount = std::pow(moves.size(), moveCount);
  }

  inline bool HasNext() {
    return currentPoss < possCount;
  }

  void GoToNext() {
    ++currentPoss;
    bool forward = true;
    for (std::size_t i = 0; i < indices.size() && forward; ++i) {
      ++indices[i];
      forward = false;
      if (indices[i] > 4) {
        indices[i] = 0;
        forward = true; 
      }
    }
  } 

  void NextInto(std::vector<std::pair<int, int>> &nextMoves) {
    for (std::size_t i = 0; i < nextMoves.size(); ++i) {
      nextMoves[i].first = moves[indices[i]].first;
      nextMoves[i].second = moves[indices[i]].second;
    }
    GoToNext();
  }

  void Reset() {
    std::fill(begin(indices), end(indices), 0);
    currentPoss = 0;
  }

};

inline bool IsFinalPositionReached(const Case &currentCase, const Config &config) {
  for (std::size_t i = 0; i < currentCase.currentPositions.size(); ++i) {
    if (currentCase.currentPositions[i] != config.finishPositions[i]) return false;
  }
  return true;
}

void bfs(Config &config) {
  // z configu stworzyć case
  Case baseCase{std::move(config.board), config.startPositions, config.dims, 0};

  // dodać go do kolejki
  std::queue<Case> queue;
  queue.push(std::move(baseCase));

  MoveCombinationProvider moveProvider(config.robotCount);
  std::vector<std::pair<int, int>> currentMoves;

  // zacząć przeglądać rozwiązania bfsem
  while (!queue.empty()) {
    Case currentCase = std::move(queue.front());
    queue.pop();

    if (currentCase.time > config.timeLimit) return;

    // skoro zawsze ma istnieć rozwiązanie to możemy najpierw sprawdzić 
    // czy finalna pozycja została osiągnięta, a dopiero potem martwić się parametrem D
    if (IsFinalPositionReached(currentCase, config)) {
      // TODO: add logic here
      std::cout << "Final position reached!\n";
      return;
    }

    moveProvider.Reset();
    while (moveProvider.HasNext()) {
      // UWAGA: TODO: TE RUCHY NIE BYŁY NIGDZIE WALIDOWANE JESZCZE
      moveProvider.NextInto(currentMoves);
      // teraz tworzymy Case dla każdego przypadku
      Case newCase {
        currentCase.board,
        currentCase.currentPositions,
        currentCase.dims,
        currentCase.time + 1
      };
      queue.push(std::move(newCase));
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

  Config config = std::move(loadInputFromStdIn());
  bfs(config);

  for (auto i = config.solution.rbegin(); i != config.solution.rend(); ++i) {
    std::cout << *i;
  }
  return 0;
}

void loadInputFromStdIn() {
  // params
  BoardDims dims;
  int robotCount;
  double minDistance;
  int timeLimit;

  std::cin >> dims.height >> dims.width;
  std::cin >> robotCount;
  std::cin >> minDistance;
  std::cin >> timeLimit;

  Config config(dims, robotCount, minDistance, timeLimit);

  std::string lineBuffer;

  // board
  for (int row = 0; row < config.dims.height; ++row) {
    std::cin >> lineBuffer;
    for (int col = 0; col < config.dims.width; ++col) {
      if (lineBuffer[col] == '.') {
        config.board[row][col].SetStatusForAll(FieldStatus::Empty);
      } else if (lineBuffer[col] == '#') {
        config.board[row][col].SetStatusForAll(FieldStatus::Blocked);
      } else if (lineBuffer[col] >= 'a' && lineBuffer[col] <= 'z') {
        char robotCharId = lineBuffer[col];
        switch (robotCharId) {
          case 'a': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Start, FieldStatus::Empty, FieldStatus::Empty);
            config.board[row][col].SetParentsForAll('X');
            config.startPositions[robotCharId - 'a'] = {row, col};
            break;
          },
          case 'b': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Empty, FieldStatus::Start, FieldStatus::Empty);
            config.board[row][col].SetParentsForAll('X');
            config.startPositions[robotCharId - 'a'] = {row, col};
            break;
          },
          case 'c': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Empty, FieldStatus::Empty, FieldStatus::Start);
            config.board[row][col].SetParentsForAll('X');
            config.startPositions[robotCharId - 'a'] = {row, col};
          },
          default: {
            std::cout << "Robot with unrecognized id!\n";
          }
        }
      } else if (lineBuffer[col] >= 'A' && lineBuffer[col] <= 'Z') {
        config.board[row][col].status = FieldStatus::Empty;
        config.finishPoint = {row, col};
      }
    }
  }
}
