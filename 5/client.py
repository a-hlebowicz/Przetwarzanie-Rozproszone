import socket
import threading
import os
import datetime

HOST = '127.0.0.1'
PORT = 12345

nick = input("Podaj swój nick: ").strip()
LOG_PATH = f"logs/client_{nick}.log"

def log(msg):
    with open(LOG_PATH, 'a') as f:
        f.write(f"{datetime.datetime.now()} [CLIENT] {msg}\n")
    print(f"[CLIENT] {msg}")

def receive_messages(sock):
    while True:
        try:
            msg = sock.recv(1024).decode().strip()
            if msg:
                log(msg)
        except:
            break

def main():
    os.makedirs("logs", exist_ok=True)
    open(LOG_PATH, 'w').close()  # clear log

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        prompt = s.recv(1024).decode()
        if prompt.startswith("NICK"):
            s.sendall(nick.encode())

        threading.Thread(target=receive_messages, args=(s,), daemon=True).start()

        while True:
            try:
                line = input()
                if '-' not in line:
                    print("Format= odbiorca-wiadomosc")
                    continue
                s.sendall(line.encode())
            except (KeyboardInterrupt, EOFError):
                print("\n[CLIENT] Zamykanie...")
                break

if __name__ == "__main__":
    main()
