from fastapi import FastAPI
from pydantic import BaseModel
import redis

app = FastAPI()

redis_client = redis.Redis(
    host = "localhost",
    port = 6379,
    decode_responses=True
)


class JobRequest(BaseModel):
    type: str
    a: int = 0
    b: int = 0

@app.get("/")
def root():
    return {"message": "Job queue api is running"}

@app.post("/jobs")
def create_job(job : JobRequest):
    job_id = redis_client.incr("job_id")
    job_data = f"{job_id}|{job.type}|{job.a}|{job.b}"
    redis_client.lpush("jobs", job_data)

    return {
        "job_id": job_id,
        "status": "queued"
    }

