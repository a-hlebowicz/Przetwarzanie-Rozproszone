import asyncio
from PIL import Image
import sys
import time

num_tasks = 8

async def negate_range(y_range, width, pixels, negatyw_pixels):
    for y in y_range:
        for x in range(width):
            r, g, b = pixels[x, y]
            negatyw_pixels[x, y] = (255 - r, 255 - g, 255 - b)
    return None

async def main():
    if len(sys.argv) != 3:
        print("python3 negaty.py obraz.jpg liczba_zadań")
        return

    filename = sys.argv[1]
    num_tasks = int(sys.argv[2])

    image = Image.open(filename).convert("RGB")
    width, height = image.size
    pixels = image.load()
    negatyw = Image.new("RGB", (width, height))
    negatyw_pixels = negatyw.load()

    start_time = time.time()

    step = height // num_tasks
    tasks = []
    for i in range(num_tasks):
        y_start = i * step
        y_end = (i + 1) * step if i != num_tasks - 1 else height
        y_range = range(y_start, y_end)
        tasks.append(asyncio.create_task(negate_range(y_range, width, pixels, negatyw_pixels)))

    await asyncio.gather(*tasks)

    negatyw.save("negatyw.jpg")
    end_time = time.time()
    print(f"Zapisano jako 'negatyw.jpg'")
    print(f"Czas: {end_time - start_time} s")

if __name__ == "__main__":
    asyncio.run(main())
