import socket
import threading
import os
import datetime

HOST = '127.0.0.1'   #lokalnie działa, /ip a
PORT = 12345
LOG_PATH = "logs/server.log"

clients = {}         # nick -> socket
queues = {}          # nick -> list of messages
lock = threading.Lock()

def log(msg):
    with open(LOG_PATH, 'a') as f:
        f.write(f"{datetime.datetime.now()} [SERVER] {msg}\n")
    print(f"[SERVER] {msg}")

def client_thread(conn, addr):
    try:
        conn.sendall(b"NICK:")                      # print(podaj nick)
        nick = conn.recv(1024).decode().strip()     # odbierz nick

        #rejestracja i stworzenie kolejki lda nicku
        with lock:
            clients[nick] = conn
            if nick not in queues:
                queues[nick] = []

        log(f"{nick} connected from {addr}")

        # Jeśli były wiadomości, wyślij je
        with lock:
            for msg in queues[nick]:
                conn.sendall(msg.encode())
            queues[nick] = []

        #GŁÓWNA PĘTLA - czekanie na wiadomosci
        while True:
            data = conn.recv(1024).decode().strip()
            if not data:                        
                break                               #koniec

            if '-' not in data:
                conn.sendall(b"Zly format\n")
                continue

            odbiorca, msg = data.split('-', 1)
            with lock:
                if odbiorca in clients:             #jezeli odbiorca online
                    try:
                        clients[odbiorca].sendall(f"{nick}- {msg}\n".encode())
                    except:                         #połączenie zerwane
                        queues[odbiorca].append(f"{nick}- {msg}\n")
                else:
                    # Dodaj do kolejki, nawet jeśli klient był tylko kiedyś
                    if odbiorca not in queues:
                        conn.sendall(b"Nieznany klient\n")
                    else:
                        queues[odbiorca].append(f"{nick}- {msg}\n")
    except Exception as e:
        log(f"Exception with client: {e}")
    finally:                #finally aby zawsze sie wykonywalo
        with lock:
            if nick in clients:
                del clients[nick]                   #klient -> offline
        conn.close()
        log(f"{nick} disconnected")

def start_server():
    os.makedirs("logs", exist_ok=True)
    open(LOG_PATH, 'w').close()  # clear log
    #tworzy socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()

        log(f"Serwer uruchomiony na {HOST}:{PORT}")

        while True:
            conn, addr = s.accept()
            #dla każdego klienta nowy wątek
            threading.Thread(target=client_thread, args=(conn, addr), daemon=True).start()

if __name__ == "__main__":
    start_server()
