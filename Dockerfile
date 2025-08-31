FROM ubuntu:20.04

# تثبيت الأدوات المطلوبة
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    ffmpeg \
    strace \
    htop \
    tcpdump \
    && rm -rf /var/lib/apt/lists/*

# نسخ ملفات المشروع
COPY src/ /app/src/
COPY planning/ /app/planning/

# بناء المشروع
WORKDIR /app
RUN g++ -o mining-simulator src/main.cpp -pthread

# نسخ XMRig و config.json (افترض وجودهما)
COPY xmrig /usr/local/bin/xmrig
COPY config.json /usr/local/bin/config.json

# تشغيل المحاكي مع مراقبة
CMD ["strace", "-f", "./mining-simulator"]
