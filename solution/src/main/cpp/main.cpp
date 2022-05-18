#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>
#include <cmath>

constexpr static int MaxRobotCount = 3;


void PrintPositions(const std::vector<std::pair<int, int>> &positions) {
  for (std::size_t i = 0; i < positions.size(); ++i) {
    std::cout << (char)(i + 'a') << " at pos: " << positions[i].first << ", " << positions[i].second << "\n";
  }
}

enum FieldStatus {
  Visited, Empty, Blocked, Start, Finish, Enqueued
};

double inline squaredDistance(const std::pair<int, int> &positionOne, const std::pair<int, int> &positionTwo) {
  std::cout << "Calculating distance between: " << positionOne.first << ", " << positionOne.second << " | " << positionTwo.first << ", " << positionTwo.second << '\n';
  return (positionOne.first - positionTwo.first) * (positionOne.first - positionTwo.first) +
      (positionOne.second - positionTwo.second) * (positionOne.second - positionTwo.second);
}

struct BoardField {
  std::array<FieldStatus, MaxRobotCount> statuses;
//  std::array<std::pair<int, int>, MaxRobotCount> parents;
  // rozważyć od razu uzupełnianie literkami (nie ma potrzeby potem tego odtwarzać)
  std::array<char, MaxRobotCount> parents;

  bool hasRobot = false;

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
  std::vector<std::string> solutions;
  std::vector<std::pair<int, int>> startPositions;
  std::vector<std::pair<int, int>> finishPositions;
  int robotCount;
  double minDistanceSquared;
  int timeLimit;

  Config(BoardDims dims, int robotCount, double minDistance, int timeLimit) : 
      dims(dims),
      robotCount(robotCount),
      minDistanceSquared(minDistance * minDistance),
      timeLimit(timeLimit) {
    board.resize(dims.height);
    for (auto &vec : board) vec.resize(dims.width);

    solutions.resize(robotCount);
    startPositions.resize(robotCount);
    finishPositions.resize(robotCount);
  }
};

struct Case {
  std::vector<std::vector<BoardField>> board;
  std::vector<std::pair<int, int>> currentPositions;
  std::vector<std::string> solutions;
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
    possCount = std::pow(moves.size(), moveCount) - 1;
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

char MapMoveToChar(const std::pair<int, int> &move) {
  if (move.first == 0) {
    if (move.second == 1) return 'P';
    else if (move.second == 0) return 'S';
    else return 'L';
  } else if (move.first == 1) {
    return 'D';
  } else if (move.first == -1) {
    return 'G';
  } else {
    std::cout << "INVALID MOVE!!!!\n";
    exit(1);
  }
}

inline bool IsFinalPositionReached(const Case &currentCase, const Config &config) {
  for (std::size_t i = 0; i < currentCase.currentPositions.size(); ++i) {
    if (currentCase.currentPositions[i] != config.finishPositions[i]) return false;
  }
  return true;
}

bool TryCommitMoves(const std::vector<std::pair<int, int>> &moves, Case &currCase, const Config &config) {
  int first, second;
  for (int robotId = 0; robotId < (int)moves.size(); ++robotId) {
    std::cout << "Attemp to commit move for robot: " << (char)(robotId + 'a') << '\n';
    first = currCase.currentPositions[robotId].first + moves[robotId].first;
    second = currCase.currentPositions[robotId].second + moves[robotId].second;
    std::cout << "\tpossible new pos: " << first << ", " << second << '\n';
    if ((first >= 0 && first < currCase.dims.height && second >= 0 && second < currCase.dims.width) &&
        (currCase.board[first][second].statuses[0] != FieldStatus::Blocked) && // 0 -- jak pole jest zablokowane, to jest zablokowane dla każdego robota
        (currCase.board[first][second].statuses[robotId] != FieldStatus::Visited))
        // (!currCase.board[first][second].hasRobot))
        // (currCase.board[first][second].statuses[robotId] != FieldStatus::Enqueued)) // do przemyślenia
    {
      // próba wykonania ruchu
      currCase.currentPositions[robotId].first = first;
      currCase.currentPositions[robotId].second = second;


      // TODO: ustawianie parentów

      // sprawdzenie czy zasada odległości nie jest naruszona
      std::cout << "Size of moves vector: " << moves.size() << '\n';
      for (int id = 0; id < (int)moves.size(); ++id) {
        // std::cout << "robotId: " << robotId << ", id: " << id << '\n';
        if (robotId != id && squaredDistance(currCase.currentPositions[robotId], currCase.currentPositions[id]) <= config.minDistanceSquared) {
          std::cout << "\tRejected: Distance violated\n";
          std::cout << "\t" << squaredDistance(currCase.currentPositions[robotId], currCase.currentPositions[id]) << " <= " << config.minDistanceSquared << '\n';
          return false;
        }
        if (robotId != id && currCase.currentPositions[id] == currCase.currentPositions[robotId]) {
          std::cout << "\tRejected: Tresspass\n";
          return false;
        }
      }
      currCase.solutions[robotId].push_back(MapMoveToChar(moves[robotId]));
      // currCase.board[first][second].hasRobot = true;
    } else {
      std::cout << "\tRejected: ";
      if ((first >= 0 && first < currCase.dims.height && second >= 0 && second < currCase.dims.width)) {
        std::cout << "Move outside bounds\n";
      } else if ((currCase.board[first][second].statuses[0] != FieldStatus::Blocked)) {
        std::cout << "Blocked\n";
      } else if ((currCase.board[first][second].statuses[robotId] != FieldStatus::Visited)) {
        std::cout << "Visited\n";
      } else if (!currCase.board[first][second].hasRobot) {
        std::cout << "Occupied\n";
      } else {
        std::cout << "Rejected for no reason\n";
      }
      return false;
    }
  }
  std::cout << "Commited the moves\n";
  return true;
}

void bfs(Config &config) {
  std::cout << "Starting bfs\n";
  // z configu stworzyć case

  std::cout << "Starting positions from config: \n";
  PrintPositions(config.startPositions);

  Case baseCase{std::move(config.board), config.startPositions, std::vector<std::string>(config.robotCount), config.dims, 0};

  // dodać go do kolejki
  std::queue<Case> queue;
  queue.push(std::move(baseCase));

  MoveCombinationProvider moveProvider(config.robotCount);
  std::vector<std::pair<int, int>> currentMoves(config.robotCount);

  // zacząć przeglądać rozwiązania bfsem
  while (!queue.empty()) {
    Case currentCase = std::move(queue.front());
    queue.pop();

    std::cout << "----------------------------------------------------\n";
    std::cout << "Considering case:\n";
    PrintPositions(currentCase.currentPositions);
    std::cout << "time: " << currentCase.time << '\n';
    std::cout << "time limit: " << config.timeLimit << '\n';
    std::cout << "--------------------------\n";


    if (currentCase.time > config.timeLimit) return;

    // skoro zawsze ma istnieć rozwiązanie to możemy najpierw sprawdzić 
    // czy finalna pozycja została osiągnięta, a dopiero potem martwić się parametrem D
    if (IsFinalPositionReached(currentCase, config)) {
      // TODO: add logic here
      std::cout << "Final position reached!\n";
      // for (auto &solution : currentCase.solutions) {
      //   std::cout << solution << "\n";
      // }
      config.solutions = std::move(currentCase.solutions);
      return;
    }

    // najpierw oznaczamy wszystkie pola jako odwiedzone w tym casie'

    for (int robotId = 0; robotId < config.robotCount; ++robotId) {
      currentCase.board[currentCase.currentPositions[robotId].first][currentCase.currentPositions[robotId].second]
          .SetStatusForRobot(robotId, FieldStatus::Visited);
    }

    std::vector<std::pair<int, int>> basePositions(config.robotCount);
    std::copy(std::begin(currentCase.currentPositions), std::end(currentCase.currentPositions), std::begin(basePositions));

    moveProvider.Reset();
    while (moveProvider.HasNext()) {
      moveProvider.NextInto(currentMoves);

      std::cout << "Considering move: ";
      for (auto &move : currentMoves) std::cout << move.first << ", " << move.second << " | ";
      std::cout << std::endl;

      // teraz trzeba sprawdzić, czy ten ruch jest możliwy do wykonania:
      if (TryCommitMoves(currentMoves, currentCase, config)) {
        std::cout << "\tAdded\n";
        Case newCase {
          currentCase.board,
          currentCase.currentPositions,
          currentCase.solutions,
          currentCase.dims,
          currentCase.time + 1
        };
        queue.push(std::move(newCase));
      }

      // przywracam pozycje wyjściowe
      std::copy(std::begin(basePositions), std::end(basePositions), std::begin(currentCase.currentPositions));
    }
  }
  std::cout << "----------------------------------------------------\n";
}

Config loadInputFromStdIn();

int main(int argc, char * argv[]) {

  Config config = loadInputFromStdIn();
  bfs(config);

  for (auto &solution : config.solutions) {
    std::cout << solution << "\n";
  }
  return 0;
}

Config loadInputFromStdIn() {
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
    std::cout << "Entered line: " << lineBuffer << '\n';
    for (int col = 0; col < config.dims.width; ++col) {
      if (lineBuffer[col] == '.') {
        config.board[row][col].SetStatusForAll(FieldStatus::Empty);
      } else if (lineBuffer[col] == '#') {
        config.board[row][col].SetStatusForAll(FieldStatus::Blocked);
      } else if (lineBuffer[col] >= 'a' && lineBuffer[col] <= 'z') {
        char robotCharId = lineBuffer[col];
        std::cout << "Detected robot " << robotCharId << " at position " << row << ", " << col << '\n';
        switch (robotCharId) {
          case 'a': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Start, FieldStatus::Empty, FieldStatus::Empty);
            break;
          }
          case 'b': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Empty, FieldStatus::Start, FieldStatus::Empty);
            break;
          }
          case 'c': {
            config.board[row][col].SetStatusesInorder(FieldStatus::Empty, FieldStatus::Empty, FieldStatus::Start);
            break;
          }
          default: {
            std::cout << "Robot with unrecognized id!\n";
          }
        }
        config.board[row][col].SetParentsForAll('X'); 
        config.startPositions[robotCharId - 'a'] = {row, col};
      } else if (lineBuffer[col] >= 'A' && lineBuffer[col] <= 'Z') {
        char robotCharId = lineBuffer[col];
        std::cout << "Detected final position for robot " << robotCharId << " at " << row << ", " << col << '\n';
        config.finishPositions[robotCharId - 'A'] = {row, col};
        config.board[row][col].SetStatusForAll(FieldStatus::Empty); // Todo: consider FieldStatus::Finish
      }
    }
  }
  // std::cout << "Positions just after config was loaded\n";
  // PrintPositions(config.startPositions);
  return config;
}
