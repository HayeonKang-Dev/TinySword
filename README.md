## Tiny Sword 
### 적군의 성을 파괴하고 마지막 1인이 되어라!


<p align="center">
<img width="1919" height="1079" alt="스크린샷 2025-08-14 181434" src="https://github.com/user-attachments/assets/3f59f74b-6acb-4a45-a0b5-b54c0dbdb0e2" />
</p>

[<img width="100" height="auto" alt="image" src="https://github.com/user-attachments/assets/e0ec78c6-7c0d-474e-9ded-ade27adac28a" />
](https://www.youtube.com/watch?v=IAcIKz_0OaE)

# 🧩 Tiny Sword - Multiplayer Strategy Game

> A 2D Multiplayer Strategy Game built with Unreal Engine and a C++ IOCP Server.  
> Developed to explore **real-time network synchronization**, **server-authoritative architecture**, and **system optimization**.

---

## 🎮 Overview
**Tiny Sword**는 언리얼 엔진 기반의 2D 멀티플레이 전략 전투 게임입니다.  
플레이어는 각자 고유한 색상의 성(Castle)을 소유하고, 자원을 모으며 적을 공격해 승리합니다.  

모든 게임 로직은 **C++ 기반 IOCP 서버**에서 처리되며,  
클라이언트는 서버와 **TCP 통신**을 통해 실시간으로 이동, 공격, NPC 상태, 아이템 획득 등을 동기화합니다.

---

## ⚙️ Tech Stack

| 구분 | 기술 |
|------|------|
| **Client** | Unreal Engine 5 (Paper2D / PaperZD), C++ |
| **Server** | C++ (Win32 IOCP, Multi-thread, Timer Thread, Critical Section) |
| **Protocol** | Custom Binary TCP Protocol |
| **Tools** | Visual Studio, Wireshark, Git, PlantUML |
| **DB** | (Optional) MariaDB – Ranking / Logging Prototype |

---

## 🏗️ Architecture Overview

### 🧠 Server Structure
서버는 **IOCP 기반 멀티스레드 구조**로 동작합니다.  
모든 소켓 이벤트(Recv/Send)는 IOCP Queue에 등록되며, Worker Thread가 이를 병렬 처리합니다.

#### 주요 구성 요소
- **Main Thread** – 클라이언트 접속 관리 및 Worker 초기화  
- **Worker Thread Pool** – IOCP 이벤트 처리, 패킷 분배  
- **Timer Thread** – 서버 주기적 송신 패킷 처리 (예: Sheep 이동, Bomb 폭발 등)  
- **Manager Classes**
  - `PlayerManager` – 유저 이동/공격/패킷 처리  
  - `SheepManager` – NPC 이동 및 충돌 감지  
  - `BombManager` – 폭발 타이머 및 범위 판정  
  - `CastleManager` – 각 팀별 성 HP/상태 관리  
  - `ItemManager` – GoldBag, Meat 등 아이템 생성 및 획득 로직  

#### Thread Relationship Diagram
```mermaid
flowchart TD
    A[Main Thread] --> B[IOCP Queue]
    B --> C[Worker Threads]
    B --> D[Timer Thread]
    C --> E[PlayerManager]
    C --> F[SheepManager]
    C --> G[BombManager]
    C --> H[CastleManager]
    C --> I[ItemManager]
