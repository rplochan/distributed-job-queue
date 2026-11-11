from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import psycopg2
import redis
import json
import os

app = FastAPI()

# PostgreSQL connection
db = psycopg2.connect(
    host="localhost",
    port=5432,
    database="job_queue",
    user="postgres",
    password=os.environ["DB_PASSWORD"]
)

# Valkey connection
redis_client = redis.Redis(
    host="localhost",
    port=6379,
    decode_responses=True
)


# Request model
class JobRequest(BaseModel):
    type: str
    a: int = 0
    b: int = 0
    duration: int = 0
    input: str = ""


# Supported job types
SUPPORTED_JOBS = {
    "add",
    "subtract",
    "mult",
    "multiply",
    "div",
    "power",
    "factorial",
    "fibonacci",
    "prime_check",
    "sleep",
    "hash",
    "pdf_export"
    "image_resize"
}


@app.get("/")
def root():
    return {"message": "Job queue API is running"}


@app.post("/jobs")
def create_job(job: JobRequest):

    # 1. Validate job type
    if job.type not in SUPPORTED_JOBS:
        raise HTTPException(
            status_code=400,
            detail="Unsupported job type"
        )

    # 2. Validate hash input
    if job.type == "hash" and not job.input:
        raise HTTPException(
            status_code=400,
            detail="Hash input cannot be empty"
        )

    # 3. Validate sleep duration
    if job.type == "sleep" and job.duration < 0:
        raise HTTPException(
            status_code=400,
            detail="Sleep duration cannot be negative"
        )

    # 4. Validate division
    if job.type == "div" and job.b == 0:
        raise HTTPException(
            status_code=400,
            detail="Division by zero"
        )

    # 5. Validate factorial
    if job.type == "factorial" and job.a < 0:
        raise HTTPException(
            status_code=400,
            detail="Factorial requires non-negative input"
        )

    # 6. Validate Fibonacci
    if job.type == "fibonacci" and job.a < 0:
        raise HTTPException(
            status_code=400,
            detail="Fibonacci requires non-negative input"
        )

    # 7. Validate power
    if job.type == "power" and job.b < 0:
        raise HTTPException(
            status_code=400,
            detail="Negative exponent unsupported"
        )

    if job.type == "image_resize":
        if not job.input.strip():
            raise HTTPException(
                status_code=400,
                detail="Input image path is required"
        )

    if job.a <= 0 or job.b <= 0:
        raise HTTPException(
            status_code=400,
            detail="Image width and height must be positive"
        )

    # 8. Generate unique job ID
    job_id = redis_client.incr("job_id")

    # 9. Prepare job arguments for PostgreSQL
    input_data = {
        "a": job.a,
        "b": job.b,
        "duration": job.duration,
        "input": job.input
    }

    # 10. Insert job into PostgreSQL
    cursor = db.cursor()

    try:
        cursor.execute(
            """
            INSERT INTO jobs (id, type, input_data, status)
            VALUES (%s, %s, %s, %s)
            """,
            (job_id, job.type, json.dumps(input_data), "pending")
        )

        db.commit()

    except Exception:
        db.rollback()
        raise

    finally:
        cursor.close()

    # 11. Serialize job for the C++ parser
    job_data = (
        f"{job_id}|{job.type}|{job.a}|{job.b}|"
        f"{job.duration}|{job.input}"
    )

    # 12. Add job to Valkey queue
    redis_client.lpush("jobs", job_data)

    # 13. Return response
    return {
        "job_id": job_id,
        "status": "pending"
    }

@app.get("/jobs/{job_id}")
def get_job_status(job_id: int):

    cursor = db.cursor()

    try:
        cursor.execute(
            """
            SELECT status, result_int, result_text, error
            FROM jobs
            WHERE id = %s
            """,
            (job_id,)
        )

        result = cursor.fetchone()

        if result is None:
            raise HTTPException(
                status_code=404,
                detail="Job not found"
            )

        status, result_int, result_text, error = result

        return {
            "job_id": job_id,
            "status": status,
            "result_int": result_int,
            "result_text": result_text,
            "error": error
        }

    finally:
        cursor.close()
